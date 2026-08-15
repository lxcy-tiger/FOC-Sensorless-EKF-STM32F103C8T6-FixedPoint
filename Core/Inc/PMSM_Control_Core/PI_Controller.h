//
// Created by lxcy on 2026/7/14.
//

#ifndef FOC_SENSORLESS_FIXED_PI_CONTROLLER_H
#define FOC_SENSORLESS_FIXED_PI_CONTROLLER_H
#include "fixed_point.h"
#include "User_Parameters.h"
//本函数用于将Q15增益一个固定的float值，并且不移位，专用于PI控制器的积分部分，以保证积分项的精度(根据float的大小自动使用不同的定点值进行相乘,适配0~2^(-10)~2^10以内的浮点数)
static __inline__ __attribute__((__always_inline__)) int64_t Q15_ITs_mul(Q15_t x, const float y, int32_t *rightMove) {
    if (my_abs(y) < 1.f / (1 << 10)) {
        *rightMove = +10;
        return (int64_t) x.parent_value * (int32_t) (y * (1 << (15 + 10)));
    }
    if (my_abs(y) < 1.f / (1 << 9)) {
        *rightMove = +9;
        return (int64_t) x.parent_value * (int32_t) (y * (1 << (15 + 9)));
    }
    if (my_abs(y) < 1.f / (1 << 8)) {
        *rightMove = +8;
        return (int64_t) x.parent_value * (int32_t) (y * (1 << (15 + 8)));
    }
    if (my_abs(y) < 1.f / (1 << 7)) {
        *rightMove = +7;
        return (int64_t) x.parent_value * (int32_t) (y * (1 << (15 + 7)));
    }
    if (my_abs(y) < 1.f / (1 << 6)) {
        *rightMove = +6;
        return (int64_t) x.parent_value * (int32_t) (y * (1 << (15 + 6)));
    }
    if (my_abs(y) < 1.f / (1 << 5)) {
        *rightMove = +5;
        return (int64_t) x.parent_value * (int32_t) (y * (1 << (15 + 5)));
    }
    if (my_abs(y) < 1.f / (1 << 4)) {
        *rightMove = +4;
        return (int64_t) x.parent_value * (int32_t) (y * (1 << (15 + 4)));
    }
    if (my_abs(y) < 1.f / (1 << 3)) {
        *rightMove = +3;
        return (int64_t) x.parent_value * (int32_t) (y * (1 << (15 + 3)));
    }
    if (my_abs(y) < 1.f / (1 << 2)) {
        *rightMove = +2;
        return (int64_t) x.parent_value * (int32_t) (y * (1 << (15 + 2)));
    }
    if (my_abs(y) < 1.f / (1 << 1)) {
        *rightMove = +1;
        return (int64_t) x.parent_value * (int32_t) (y * (1 << (15 + 1)));
    }
    if (my_abs(y) < (float) (1 << 0)) {
        *rightMove = +0;
        return (int64_t) x.parent_value * (int32_t) (y * (1 << (15 + 0)));
    }
    if (my_abs(y) < (float) (1 << 1)) {
        *rightMove = -1;
        return (int64_t) x.parent_value * (int32_t) (y * (1 << (15 - 1)));
    }
    if (my_abs(y) < (float) (1 << 2)) {
        *rightMove = -2;
        return (int64_t) x.parent_value * (int32_t) (y * (1 << (15 - 2)));
    }
    if (my_abs(y) < (float) (1 << 3)) {
        *rightMove = -3;
        return (int64_t) x.parent_value * (int32_t) (y * (1 << (15 - 3)));
    }
    if (my_abs(y) < (float) (1 << 4)) {
        *rightMove = -4;
        return (int64_t) x.parent_value * (int32_t) (y * (1 << (15 - 4)));
    }
    if (my_abs(y) < (float) (1 << 5)) {
        *rightMove = -5;
        return (int64_t) x.parent_value * (int32_t) (y * (1 << (15 - 5)));
    }
    if (my_abs(y) < (float) (1 << 6)) {
        *rightMove = -6;
        return (int64_t) x.parent_value * (int32_t) (y * (1 << (15 - 6)));
    }
    if (my_abs(y) < (float) (1 << 7)) {
        *rightMove = -7;
        return (int64_t) x.parent_value * (int32_t) (y * (1 << (15 - 7)));
    }
    if (my_abs(y) < (float) (1 << 8)) {
        *rightMove = -8;
        return (int64_t) x.parent_value * (int32_t) (y * (1 << (15 - 8)));
    }
    if (my_abs(y) < (float) (1 << 9)) {
        *rightMove = -9;
        return (int64_t) x.parent_value * (int32_t) (y * (1 << (15 - 9)));
    }
    if (my_abs(y) < (float) (1 << 10)) {
        *rightMove = -10;
        return (int64_t) x.parent_value * (int32_t) (y * (1 << (15 - 10)));
    }
    *rightMove = 0;
    return 0;
}
//防止溢出的int32加法
static inline int32_t sat_add_i32(int32_t a, int32_t b)
{
    int32_t result;
    if (__builtin_add_overflow(a, b, &result)) {
        // 发生溢出，a 和 b 必为同号，按符号饱和
        return (a < 0) ? INT32_MIN : INT32_MAX;
    }
    return result;
}
//防止溢出的int64加法(积分累加器)
static inline int64_t sat_add_i64(int64_t a, int64_t b)
{
    int64_t result;
    if (__builtin_add_overflow(a, b, &result)) {
        return (a < 0) ? INT64_MIN : INT64_MAX;
    }
    return result;
}
//将int64的积分累加值右移后饱和到Q15范围(-32768~32767)
static inline int32_t sat_q15_i64(int64_t x) {
    if (x > 32767) return 32767;
    if (x < -32768) return -32768;
    return (int32_t) x;
}
/*
 *使用宏定义生成代码:
 *(NAME函数名称,P_VAL比例参数,I_Ts_VAL积分参数(I*Ts类型),
 * MAX_VAL输出最大值,MIN_VAL输出最小值,CalculateError,为1时计算error=Set-Measure,为0时error由用户给定)
 * Input_QType为输入量(以及中间量)的定点类型
 * Output_QType为输出量的定点类型
 * PI控制器非常容易溢出，所以这里额外使用了satInt32函数来限制溢出。
 */
#define GenerateFunction_PIController(NAME, P_VAL, I_Ts_VAL, MAX_VAL, MIN_VAL, CalculateError,Input_QType,Output_QType) \
struct PI_Controller_##NAME##_t {                             \
    Q15_##Input_QType##_t Set;/*设定值*/                         \
    Q15_##Input_QType##_t Measure;/*实测值*/                                   \
    Q15_t error;/*误差值,由于误差值是中间计算结果,容易溢出,因此这里采用int32_t存储*/               \
    /*error=Set-Measure*/                                     \
    Q15_##Output_QType##_t P_Output_Part;/*输出值的比例部分*/                                    \
    int64_t I_Output_SinglePart;/*输出值的单次积分计算值部分*/                                    \
    int64_t I_Output_Part;/*输出值的积分部分*/                                    \
    int32_t I_RightMove;/*积分部分需要右移或左移多少位得到Q15输出结果*/\
    Q15_##Output_QType##_t Output;/*输出值*/                                    \
};                                                            \
static inline void NAME##_PI_update(struct PI_Controller_##NAME##_t* NAME){ \
    /*CalculateError为0时，用户给定error，直接用用户给定的error计算，我们不会计算并覆盖掉它*/\
    if(CalculateError)NAME->error =Q15_sub(Q15_C2P(NAME->Set) , Q15_C2P(NAME->Measure)); \
    NAME->P_Output_Part=(Q15_##Output_QType##_t){satInt32(Q15_gain(NAME->error,P_VAL*Input_QType##_q15base.base_value/Output_QType##_q15base.base_value).parent_value)};\
    NAME->I_Output_SinglePart=Q15_ITs_mul(NAME->error,I_Ts_VAL*Input_QType##_q15base.base_value/Output_QType##_q15base.base_value, &(NAME->I_RightMove));\
    Q15_t output_unsat=Q15_add(_P(NAME->P_Output_Part),(Q15_t){sat_q15_i64(NAME->I_Output_Part >> (15 + NAME->I_RightMove))}); \
    Q15_##Output_QType##_t output; \
    int32_t saturated = 0; \
    if (output_unsat.parent_value > Q15_FromValue(MAX_VAL,Output_QType).child_value) { \
        output = Q15_FromValue(MAX_VAL,Output_QType); \
        saturated = 1; \
    } else if (output_unsat.parent_value < Q15_FromValue(MIN_VAL,Output_QType).child_value) { \
        output = Q15_FromValue(MIN_VAL,Output_QType); \
        saturated = -1; \
    } else { \
        output = Q15_C2C(output_unsat,Output_QType); \
    } \
    /* 积分不饱和时允许积分，或者误差符号有助于减少积分饱和时也允许积分 */\
    /*下一行代码等价于 if (saturated==0 || (saturated==1 && NAME->error<0) || (saturated==-1&&NAME->error>0)) {*/ \
    if(saturated * NAME->error.parent_value <= 0){\
        NAME->I_Output_Part = sat_add_i64(NAME->I_Output_Part, NAME->I_Output_SinglePart); \
    } \
        NAME->Output = output; \
    } \
    extern struct PI_Controller_##NAME##_t NAME##_PIstate;

#define Current_P_VAL (Q15_L_GetValue(Ls)*Current_wc)
#define Current_I_Ts_VAL (Q15_Z_GetValue(Rs)*Current_wc*Q15_T_GetValue(T_s))

GenerateFunction_PIController(Id, Current_P_VAL, Current_I_Ts_VAL, Current_MaxOutput, -Current_MaxOutput, 1, I, U);
GenerateFunction_PIController(Iq, Current_P_VAL, Current_I_Ts_VAL, Current_MaxOutput, -Current_MaxOutput, 1, I, U);

#define Speed_P_VAL (2*J*Speed_wc/3/POLE_PAIRS/Q15_flux_GetValue(flux))
#define Speed_I_Ts_VAL (2*J*Speed_wc*Speed_wc*Speed_wc/3/POLE_PAIRS/Q15_flux_GetValue(flux)/Current_wc*Q15_T_GetValue(T_s)*10)

GenerateFunction_PIController(Speed, Speed_P_VAL, Speed_I_Ts_VAL, Speed_MaxOutput, -Speed_MaxOutput, 1, we, I);

GenerateFunction_PIController(FluxObserver_PLLSpeed, FluxPLL_wc/0.6436*2/Q15_flux_GetValue(flux),
                              (FluxPLL_wc/0.6436)*(FluxPLL_wc/0.6436)/Q15_flux_GetValue(flux)*Q15_T_GetValue(T_s),
                              FluxPLL_MaxOutput, -FluxPLL_MaxOutput,
                              0, flux, we);

#endif //FOC_SENSORLESS_FIXED_PI_CONTROLLER_H
