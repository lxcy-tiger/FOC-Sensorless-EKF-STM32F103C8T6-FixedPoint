//
// Created by lxcy on 2025/11/30.
//
#include "PMSM_Control_Core/SVPWM.h"
static float Udc = 12;
static float DivUdc = 1.f/12;//母线电压倒数(随时更新)
static R5_t Udc_R = R5_fromFloat(12.f);
static R0_t DivUdc_R = R0_fromFloat(1/12.f);
void SetUdc(float newUdc){
    Udc=newUdc;
    DivUdc=1.f/newUdc;
    Udc_R=R5_fromFloat(Udc);
    DivUdc_R=R0_fromFloat(DivUdc);
}//更新Udc和DivUdc唯一接口
float GetUdc() {
    return Udc;
}//获取Udc唯一接口

static constexpr uint16_t HRTIM_Period = 2400; //TIMER1的周期值(事实上是1800-1)
//将Ts设为计数值的两倍，那么A、C、D的比较值恰好等于计算出来的Ton，注意，这里要用U32，不然会溢出
static constexpr uint32_t Ts = HRTIM_Period * 2;
static constexpr uint16_t CompareMin = 300; //计数器最小计数值
static constexpr uint16_t CompareMaxOffset=10;//计数器最大计数值偏移(Max-CompareMaxOffset)
static constexpr float Ts_Sqrt3=Ts*Sqrt_3;

//计算SVPWM并更新到HRTIM的计数值(内部实现简单非线性限幅，建议在外部对Valpha和Vbeta进行等比例限幅，以避免过调制)
void SVPWM_Calculate_Set(R4_t Valpha, R4_t Vbeta) {
    if (Udc_R.value == 0)return; //避免后续除法导致错误。

    //扇区(Sector)判断，A=Vbeta,B=(sqrt(3)*Valpha-Vbeta)/2,C=(-sqrt(3)*Valpha-Vbeta)/2
    //N=A+2B+4C
    //Sector(N)=1(3),2(1),3(5),4(4),5(6),6(2)
    const uint32_t Sector_Table[7] = {0, 2, 6, 1, 4, 3, 5}; //查表确定扇区，第一个值没用
    const R4_t A = Vbeta;
    const R4_t B=R4_f0_4_mul(R4_sub( R4_f1_4_mul(Valpha,Sqrt_3),Vbeta),0.5f);
    const R4_t C =R4_f0_4_mul(R4_sub( R4_f1_4_mul(Valpha,-Sqrt_3),Vbeta),0.5f);
    uint32_t N = 0;
    if (R4_float_greater(A,0))N+=1;
    if (R4_float_greater(B,0))N+=2;
    if (R4_float_greater(C,0))N+=4;
    const uint32_t Sector = Sector_Table[N];
    const R15_t sq3_Ts_divUdc =R0_f15_15_mul(DivUdc_R,Ts_Sqrt3);
    const R15_t X = R15_4_15_mul(sq3_Ts_divUdc,A)
        , Y =R15_4_15_mul(R15_neg(sq3_Ts_divUdc) , C)
        , Z = R15_4_15_mul(R15_neg(sq3_Ts_divUdc) , B);
    R15_t Tx, Ty;
    switch (Sector) {
        case 1:
            Tx = R15_neg(Z);
            Ty = X;
            break;
        case 2:
            Tx = Z;
            Ty = Y;
            break;
        case 3:
            Tx = X;
            Ty = R15_neg(Y);
            break;
        case 4:
            Tx = R15_neg(X);
            Ty = Z;
            break;
        case 5:
            Tx = R15_neg(Y);
            Ty = R15_neg(Z);
            break;
        case 6:
            Tx = Y;
            Ty = R15_neg(X);
            break;
        default:
            Tx = R15_fromFloat(0);
            Ty = R15_fromFloat(0);
            break;
    }

    uint32_t small = (Ts - R15_toInt32(Tx) - R15_toInt32(Ty)) / 4,
        medium = (Ts + R15_toInt32(Tx) - R15_toInt32(Ty)) / 4,
        big = (Ts + R15_toInt32(Tx) + R15_toInt32(Ty)) / 4;
    small = clamp_u32(small, CompareMin, HRTIM_Period - CompareMaxOffset);
    medium = clamp_u32(medium, CompareMin, HRTIM_Period - CompareMaxOffset);
    big = clamp_u32(big, CompareMin, HRTIM_Period - CompareMaxOffset);
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
            TIM1_Set_PWMCompare(HRTIM_Period - CompareMin, HRTIM_Period - CompareMin, HRTIM_Period - CompareMin);
            break;
    }
}