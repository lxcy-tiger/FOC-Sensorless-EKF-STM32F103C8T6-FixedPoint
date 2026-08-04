//
// Created by lxcy on 2026/7/12.
//
#include "PMSM_Control_Core/SVPWM.h"
#include "other.h"
static const uint16_t TIM1_Period = 2400; //TIMER1的周期值(事实上是2400-1)
//将Ts设为计数值的两倍，那么A、C、D的比较值恰好等于计算出来的Ton，注意，这里要用U32，不然会溢出
static const uint32_t Ts = TIM1_Period * 2;
static const uint16_t CompareMin = 100; //计数器最小计数值
static const uint16_t CompareMaxOffset=10;//计数器最大计数值偏移(Max-CompareMaxOffset)

//计算SVPWM并更新到TIM1的计数值(内部实现简单非线性限幅，建议在外部对Valpha和Vbeta进行等比例限幅，以避免过调制)
void SVPWM_Calculate_Set(Q15_U_t Valpha, Q15_U_t Vbeta) {
    //扇区(Sector)判断，A=Vbeta,B=(sqrt(3)*Valpha-Vbeta)/2,C=(-sqrt(3)*Valpha-Vbeta)/2
    //N=A+2B+4C
    //Sector(N)=1(3),2(1),3(5),4(4),5(6),6(2)
    const uint32_t Sector_Table[7] = {0, 2, 6, 1, 4, 3, 5}; //查表确定扇区，第一个值没用
    const Q15_U_t A = Vbeta;
    const Q15_U_t B = Q15_C2C(Q15_gain(Q15_sub( Q15_gain(Q15_C2P(Valpha),Sqrt_3),Q15_C2P(Vbeta)),0.5f),U);
    const Q15_U_t C =Q15_C2C(Q15_gain(Q15_sub( Q15_gain(Q15_C2P(Valpha),-Sqrt_3),Q15_C2P(Vbeta)),0.5f),U);
    uint32_t N = 0;
    if (A.child_value>0)N+=1;
    if (B.child_value>0)N+=2;
    if (C.child_value>0)N+=4;
    const uint32_t Sector = Sector_Table[N];
    /*
     * X=A_float*sqrt(3)*Ts/Udc,其中A_float表示A的定点值对应的真实浮点值,而这里Q15_U_t表示的A,本身就是A.child_value=A_float/U_q15base.base_value*32768
     * 而我们要计算X,则X=A.child_value*sqrt(3)*Ts*U_q15base.base_value/Udc/32768
     * 由于后面那一个乱七八糟的数都是常数，因此可以优化为X=Q15gain(A,sqrt(3)*Ts*U_q15base.base_value/Udc/32768.f)
     * 然后X直接取parent_value即可,Y,Z同理
     */
    const int16_t X = Q15_gain(Q15_C2P(A),Sqrt_3*Ts*U_q15base.base_value/Udc/32768.f).parent_value;
    const int16_t Y =Q15_gain(Q15_C2P(C),-Sqrt_3*Ts*U_q15base.base_value/Udc/32768.f).parent_value;
    const int16_t Z =Q15_gain(Q15_C2P(B),-Sqrt_3*Ts*U_q15base.base_value/Udc/32768.f).parent_value;
    uint16_t Tx, Ty;
    switch (Sector) {
        case 1:
            Tx = -Z;
            Ty = X;
            break;
        case 2:
            Tx = Z;
            Ty = Y;
            break;
        case 3:
            Tx = X;
            Ty = -Y;
            break;
        case 4:
            Tx = -X;
            Ty = Z;
            break;
        case 5:
            Tx = -Y;
            Ty = -Z;
            break;
        case 6:
            Tx = Y;
            Ty = -X;
            break;
        default:
            Tx = 0;
            Ty = 0;
            break;
    }

    uint32_t small = (Ts - Tx - Ty) / 4,
        medium = (Ts + Tx - Ty) / 4,
        big = (Ts + Tx + Ty) / 4;
    small = clamp_u32(small, CompareMin, TIM1_Period - CompareMaxOffset);
    medium = clamp_u32(medium, CompareMin, TIM1_Period - CompareMaxOffset);
    big = clamp_u32(big, CompareMin, TIM1_Period - CompareMaxOffset);
    switch (Sector) {
        case 1:
            TIM1_Set_PWMCompare(small, medium, big);
            break;
        case 2:
            TIM1_Set_PWMCompare(medium, small, big);
            break;
        case 3:
            TIM1_Set_PWMCompare(big, small, medium);
            break;
        case 4:
            TIM1_Set_PWMCompare(big, medium, small);
            break;
        case 5:
            TIM1_Set_PWMCompare(medium, big, small);
            break;
        case 6:
            TIM1_Set_PWMCompare(small, big, medium);
            break;
        default:
            //未知情况，将输出减到最小。
            TIM1_Set_PWMCompare(TIM1_Period - CompareMin, TIM1_Period - CompareMin, TIM1_Period - CompareMin);
            break;
    }
}