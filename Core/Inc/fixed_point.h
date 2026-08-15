//
// Created by lxcy on 2026/7/12.
//

#ifndef FOC_SENSORLESS_FIXED_FIXED_POINT_H
#define FOC_SENSORLESS_FIXED_FIXED_POINT_H
#include <stdint.h>


static __inline__ __attribute__((__always_inline__)) float my_abs(float input) {
    if (input < 0) return -input;
    else return input;
}

typedef struct {
    int32_t parent_value;
} Q15_t;

typedef struct {
    const float base_value;
} Q15_base_t;

#define Q15_baseValue(base_Name,base_Value)                                 \
static const Q15_base_t base_Name##_q15base={base_Value};                      \
typedef struct {                                                            \
    int16_t child_value;                                                    \
}Q15_##base_Name##_t;                                                       \
static inline float Q15_##base_Name##_GetValue(Q15_##base_Name##_t q15_var) \
{                                                                           \
    return q15_var.child_value*base_Name##_q15base.base_value/32768.f;         \
}

static inline int16_t satInt32(int x) {
    if (x > 32767 )return 32767;
    if (x<-32768)return -32768;
    return x;
}

#define Q15_FromValue(x,base_Name) (Q15_##base_Name##_t){(int16_t)(x/base_Name##_q15base.base_value*32768.f)}
#define Q15_ConvertToParent(x) (Q15_t){(x).child_value}
#define Q15_C2P(x) Q15_ConvertToParent(x)
#define Q15_ConvertToChild(x,base_Name) (Q15_##base_Name##_t){(x).parent_value}
#define Q15_C2C(x,base_Name) Q15_ConvertToChild(x,base_Name)
//更简洁的转Q15_t定义,用于复杂计算式中缩短代码长度
#define _P(x) Q15_C2P(x)
//Q15加法,得到x+y的结果
static inline Q15_t Q15_add(Q15_t x, Q15_t y) {
    return (Q15_t){x.parent_value + y.parent_value};
}

//Q15减法,得到x-y的结果
static inline Q15_t Q15_sub(Q15_t x, Q15_t y) {
    return (Q15_t){x.parent_value - y.parent_value};
}

//Q15乘法,得到x*y的结果
static inline Q15_t Q15_mul(Q15_t x, Q15_t y) {
    return (Q15_t){(int32_t) x.parent_value * y.parent_value >> 15};
}

//本函数用于将Q15增益一个固定的float值(根据float的大小自动使用不同的定点值进行相乘,适配0~2^(-10)~2^10以内的浮点数)
static __inline__ __attribute__((__always_inline__)) Q15_t Q15_gain(Q15_t x, const float y) {
    /*
     *注:以下代码只能用很多的if而不能简写成
     *for(..i..){
     *  if(..(1<<i))
     *      return (Q15_t){... y*(1<<(15-i))>>(15-i)};
     *}
     *因为这种情况下，-O2无法for展开(遗憾的是编译器还没有智能到这种程度......)，从而引入float运算，使得单片机速度降低
     *只能手动展开为if
     */
    if (my_abs(y) < 1.f / (1 << 10))
        return (Q15_t){(int32_t) x.parent_value * (int32_t) (y * (1 << (15 + 10))) >> (15 + 10)};
    if (my_abs(y) < 1.f / (1 << 9))
        return (Q15_t){(int32_t) x.parent_value * (int32_t) (y * (1 << (15 + 9))) >> (15 + 9)};
    if (my_abs(y) < 1.f / (1 << 8))
        return (Q15_t){(int32_t) x.parent_value * (int32_t) (y * (1 << (15 + 8))) >> (15 + 8)};
    if (my_abs(y) < 1.f / (1 << 7))
        return (Q15_t){(int32_t) x.parent_value * (int32_t) (y * (1 << (15 + 7))) >> (15 + 7)};
    if (my_abs(y) < 1.f / (1 << 6))
        return (Q15_t){(int32_t) x.parent_value * (int32_t) (y * (1 << (15 + 6))) >> (15 + 6)};
    if (my_abs(y) < 1.f / (1 << 5))
        return (Q15_t){(int32_t) x.parent_value * (int32_t) (y * (1 << (15 + 5))) >> (15 + 5)};
    if (my_abs(y) < 1.f / (1 << 4))
        return (Q15_t){(int32_t) x.parent_value * (int32_t) (y * (1 << (15 + 4))) >> (15 + 4)};
    if (my_abs(y) < 1.f / (1 << 3))
        return (Q15_t){(int32_t) x.parent_value * (int32_t) (y * (1 << (15 + 3))) >> (15 + 3)};
    if (my_abs(y) < 1.f / (1 << 2))
        return (Q15_t){(int32_t) x.parent_value * (int32_t) (y * (1 << (15 + 2))) >> (15 + 2)};
    if (my_abs(y) < 1.f / (1 << 1))
        return (Q15_t){(int32_t) x.parent_value * (int32_t) (y * (1 << (15 + 1))) >> (15 + 1)};
    if (my_abs(y) < (float) (1 << 0))
        return (Q15_t){(int32_t) x.parent_value * (int32_t) (y * (1 << (15 - 0))) >> (15 - 0)};
    if (my_abs(y) < (float) (1 << 1))
        return (Q15_t){(int32_t) x.parent_value * (int32_t) (y * (1 << (15 - 1))) >> (15 - 1)};
    if (my_abs(y) < (float) (1 << 2))
        return (Q15_t){(int32_t) x.parent_value * (int32_t) (y * (1 << (15 - 2))) >> (15 - 2)};
    if (my_abs(y) < (float) (1 << 3))
        return (Q15_t){(int32_t) x.parent_value * (int32_t) (y * (1 << (15 - 3))) >> (15 - 3)};
    if (my_abs(y) < (float) (1 << 4))
        return (Q15_t){(int32_t) x.parent_value * (int32_t) (y * (1 << (15 - 4))) >> (15 - 4)};
    if (my_abs(y) < (float) (1 << 5))
        return (Q15_t){(int32_t) x.parent_value * (int32_t) (y * (1 << (15 - 5))) >> (15 - 5)};
    if (my_abs(y) < (float) (1 << 6))
        return (Q15_t){(int32_t) x.parent_value * (int32_t) (y * (1 << (15 - 6))) >> (15 - 6)};
    if (my_abs(y) < (float) (1 << 7))
        return (Q15_t){(int32_t) x.parent_value * (int32_t) (y * (1 << (15 - 7))) >> (15 - 7)};
    if (my_abs(y) < (float) (1 << 8))
        return (Q15_t){(int32_t) x.parent_value * (int32_t) (y * (1 << (15 - 8))) >> (15 - 8)};
    if (my_abs(y) < (float) (1 << 9))
        return (Q15_t){(int32_t) x.parent_value * (int32_t) (y * (1 << (15 - 9))) >> (15 - 9)};
    if (my_abs(y) < (float) (1 << 10))
        return (Q15_t){(int32_t) x.parent_value * (int32_t) (y * (1 << (15 - 10))) >> (15 - 10)};

    return (Q15_t){0};
}

//Q15取相反数,得到-x的结果
static inline Q15_t Q15_neg(Q15_t x) {
    return (Q15_t){-x.parent_value};
}

//更简洁的取反 转换Q15_t定义
#define _NP(x) Q15_neg(_P(x))
//Q15三数连加,得到x+y+z的结果,可搭配neg得到除法
static inline Q15_t Q15_aa(Q15_t x, Q15_t y, Q15_t z) {
    return Q15_add(x, Q15_add(y, z));
}

//Q15三数连乘,得到x*y*z的结果,可搭配neg得到前导负号
static inline Q15_t Q15_mm(Q15_t x, Q15_t y, Q15_t z) {
    return Q15_mul(Q15_mul(x, y), z);
}

//Q15乘加,得到ax*x+ay*y的结果，可搭配neg更灵活计算
static inline Q15_t Q15_mam(Q15_t ax, Q15_t x, Q15_t ay, Q15_t y) {
    return Q15_add(Q15_mul(ax, x), Q15_mul(ay, y));
}

//Q15三数乘加,得到ax*x+ay*y+az*z的结果，可搭配neg更灵活计算
static inline Q15_t Q15_mamam(Q15_t ax, Q15_t x, Q15_t ay, Q15_t y, Q15_t az, Q15_t z) {
    return Q15_aa(Q15_mul(ax, x), Q15_mul(ay, y), Q15_mul(z, az));
}

//位移法对u32开方
static inline uint32_t sqrt_bitwise_u32(uint32_t n) {
    if (n == 0) return 0;
    uint32_t root = 0;
    uint32_t bit = 1U << 30; // (2^15)^2 = 2^30，因为 (2^16)^2 = 2^32 超出 uint32 范围
    // 对齐最高有效平方位
    while (bit > n) {
        bit >>= 2;
    }
    // 逐位确定平方根
    while (bit != 0) {
        if (n >= root + bit) {
            n -= root + bit;
            root = (root >> 1) + bit;
        } else {
            root >>= 1;
        }
        bit >>= 2;
    }
    return root;
}

//平方求和再开根,相当于(x^2+y^2)^0.5
static inline Q15_t Q15_module(Q15_t x, Q15_t y) {
    return (Q15_t){
        sqrt_bitwise_u32(
            (uint32_t) ((int32_t) x.parent_value * x.parent_value) + (uint32_t) (
                (int32_t) y.parent_value * y.parent_value))
    };
}
#endif //FOC_SENSORLESS_FIXED_FIXED_POINT_H
