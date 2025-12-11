//
// Created by lxcy on 2025/11/30.
//

#ifndef FOC_SENSORLESS_FIXED_OTHER_H
#define FOC_SENSORLESS_FIXED_OTHER_H
#include "fixed_point.h"

// 数学类常数与函数
constexpr float PI=3.1415926535897932384626433832795f;
constexpr float PI2=PI*2;
constexpr float PIdiv2=PI/2;
constexpr float Sqrt_3=1.7320508075688772935274463415059f;//根号三(编译器自动截断)
constexpr float Sqrt_3_Half=Sqrt_3/2;//二分之根号三
constexpr float Div_Sqrt_3=1/Sqrt_3;//倒数_根号三(优化除法为乘法)


// PI*2^(31-4)=421,657,428.2(截尾)=421,657,428
constexpr R4_t PI_R4=R4_fromFloat(PI);
// 2PI*2^(31-4)=843,314,856.5(截尾)=843,314,856
constexpr R4_t PI2_R4=R4_fromFloat(PI2);
// PI/2*2^(31-4)=210,828,714.1(截尾)=210,828,714
constexpr R4_t PIdiv2_R4=R4_fromFloat(PIdiv2);

//对u32数值进行限幅
static inline uint32_t clamp_u32(uint32_t input,uint32_t min,uint32_t max) {
    if (input < min)return min;
    if (input > max)return max;
    return input;
}

// 求正弦值和余弦值
void R4_sin_cos(R4_t theta,R0_t *sinTheta,R0_t *cosTheta);

// 取5个数的中位数（第3小的数）
static inline int32_t median_5(int32_t a, int32_t b, int32_t c, int32_t d, int32_t e) {
    // 使用比较网络找出中位数，避免排序
    if (a > b) { int32_t t = a; a = b; b = t; }
    if (c > d) { int32_t t = c; c = d; d = t; }
    if (a > c) { int32_t t = a; a = c; c = t; }
    if (b > d) { int32_t t = b; b = d; d = t; }
    if (b > c) { int32_t t = b; b = c; c = t; }
    // 此时 a <= b <= c <= d，e 是独立的
    if (e < b) {
        if (e < a) return b;
        return e;
    }
    if (e < c) return e;
    return c;
}
#define GenerateFunction_MedianFilter5(NAME) \
static R2_t NAME##_buf[5] = {0}; \
static uint8_t NAME##_buf_index = 0; \
R2_t median_filter_##NAME##_5(R2_t NAME##_input) { \
NAME##_buf[NAME##_buf_index] = NAME##_input; \
NAME##_buf_index = (NAME##_buf_index + 1) % 5; \
return (R2_t){median_5(NAME##_buf[0].value, NAME##_buf[1].value, NAME##_buf[2].value, NAME##_buf[3].value, NAME##_buf[4].value)}; \
}
R2_t median_filter_Ia_5(R2_t Ia_input);
R2_t median_filter_Ib_5(R2_t Ib_input);
#endif //FOC_SENSORLESS_FIXED_OTHER_H