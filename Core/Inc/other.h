//
// Created by lxcy on 2026/7/12.
//

#ifndef FOC_SENSORLESS_FIXED_OTHER_H
#define FOC_SENSORLESS_FIXED_OTHER_H
#include "PMSM_Control_Core/User_Parameters.h"
// 数学类常数与函数
static const float Sqrt_3=1.7320508075688772935274463415059f;//根号三(编译器自动截断)
static const float Sqrt_3_Half=Sqrt_3/2;//二分之根号三
static const float Div_Sqrt_3=1/Sqrt_3;//倒数_根号三(优化除法为乘法)
static const float PI=3.14159265358979323846;
static const float PI2=PI*2;
//对u32数值进行限幅
static inline uint32_t clamp_u32(uint32_t input,uint32_t min,uint32_t max) {
    if (input < min)return min;
    if (input > max)return max;
    return input;
}
void Q15_sin_cos(Q15_te_t theta,Q15_t *sinTheta,Q15_t *cosTheta);

//自动生成函数(简单的一阶低通滤波器,参数NAME:函数名称,f_c截止频率(hz),Q_type定点类型)
#define GenerateFunction_LowPassFilter(NAME,f_c,Q_type) \
    static Q15_##Q_type##_t lowPass_filter_##NAME(Q15_##Q_type##_t NAME##_input){\
        static Q15_##Q_type##_t y_old={0};\
        const float k= PI2*Q15_T_GetValue(T_s)*f_c;\
        y_old=Q15_C2C(Q15_add(Q15_gain(_P(NAME##_input),k),Q15_gain(_P(y_old),(1-k))),Q_type);\
        return y_old;\
    }

GenerateFunction_LowPassFilter(flux_Espeed,500,we)

GenerateFunction_LowPassFilter(Ia,2000,I)
GenerateFunction_LowPassFilter(Ib,2000,I)

#endif //FOC_SENSORLESS_FIXED_OTHER_H
