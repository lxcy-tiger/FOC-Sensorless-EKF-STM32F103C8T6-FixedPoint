//
// Created by lxcy on 2025/11/30.
//

#ifndef FOC_SENSORLESS_FIXED_FIX_POINT_H
#define FOC_SENSORLESS_FIXED_FIX_POINT_H
#include "main.h"
// 定点数实现(有符号)该定点数实现与常规实现(常规实现Qx中的x表示小数位数)不一样，(该实现的)Rx中的x表示整数位数的最大值<2^x
// 其中根据浮点数 float x的最大整数范围(绝对值)确定使用R几的浮点数，
// 如 x最大值为1000<1024=2^10(建议留一定裕量)，那么使用R10表示x比较合适
// 转换关系:
// R10 x_r = float x * 2^(31-10)
// 此定点数版本旨在减小溢出的概率，提升可读性
// 命名解释:为与Qx区分，这里使用Rx，其中R表示Range(范围)，Rx_t表示可以表达-2^x~2^x(不含边界)的浮点数
// 如R0_t表示这个数一定小于2^0=1，R2_t可以表示-4~4的数之类的
// 使用时根据数的大小选择合适的类型，可选择比最大值大一倍的类型，既能避免溢出也能保证精度，如
// 使用R2_t表示-0.9~0.9的数，而不是使用R1_t，这样能够避免溢出，并且精度也能达到2^(2-31)=0.0000000019,0.0000000019/0.9=2E-7%
// 本定点数，除R0_inv外，均使用C#代码自动生成，以避免出错，命名规则为:
// --------------------------------------命名规则--------------------------------------------
//      类型Rx_t{int32_t value}
//      浮点转定点宏Rx_fromFloat()
//      浮点转定点宏(有偏移项)Rx_fromFloat_offset()
//      定点转浮点宏(C11特性智能判断类型并转换)cvtRToFloat()该宏事实上只在调试的时候用,运行时根本没有需要将Rx_t转成浮点的需求
//      定点数加法Rx_add_y(),其中Rx表示Rx_t的定点数加法,y表示有多少个定点数相加(y=2时不写出),返回结果为Rx_t类型
//      定点数减法Rx_sub_y(),其中Rx表示Rx_t的定点数减法,y表示有多少个定点数相减(y=2时不写出)
//                  第一个参数的符号为"+",此后所有参数都为"-",即a-b-c-d-……,返回结果为Rx_t类型
//      定点数乘法Rx_y_z_mul(),其中Rx表示第一个参数的类型,Ry表示第二个参数的类型,Rz表示返回参数的类型
//                  如R8_1_4_mul()表示输入一个R8_t,R1_t得到R4_t
//                  注意为什么要提升类型呢？a*b,如果a<2^8,b<2^1,那么结果应当<2^8*2^1=2^9,使用R9_t合适？
//                  并不正确,有时为了防止溢出使用R8_t表示最大仅为16的数,而R1表示最大为0.5的数，得到的结果为8,使用R4_t是精度和防止溢出的平衡选择
//      定点数浮点数乘法Rx_fy_z_mul(),其中Rx表示第一个参数的类型,Ry表示浮点数要转成的定点数的类型,Rz表示返回参数的类型
//                  为了参数更加清晰,我们一般使用原始值(float)类型表示参数,并且加上C23标准的constexpr,而不是使用定点数表示
//                  如constexpr float Rs=5.5f/2; //电阻(Ohm)
//                  此时如果有定点数(比如说R4_t)要乘以这个浮点,如Rs,你要选择一个合适的类型,比如Rs=2.75f,选择R2_t是合适的
//                  我们分析一下这个函数
//                  static inline R6_t R4_f2_6_mul(R4_t x,float y) {
//                      return (R6_t){(int32_t)(((int64_t)x.value*((int32_t)(y*(float)(1U<<(31-2)))))>>(31+6-(4+2)))};
//                  }
//                  在全局O2优化级别下，float y的输入，也就是这一部分:
//                  (int32_t)(y*(float)(1U<<(31-2)))))=(int32_t)(2.75f*(float)(1U<<(31-2))))
//                  将会被优化为(int32_t)1476395008,这一整个过程都在编译期完成,运行中没有浮点乘法,保证了可读性的同时也没有牺牲速度
//                  这里不建议输入非constexpr的float,否则会卡成蜗牛！而变量尽量使用R格式,比如EKF中的static R0_t P_k_k_0_0=R0_fromFloat(0.03)
//      定点数浮点数加法Rx_float_add()，和上面乘法的目的相同，但是这里输入，输出，和浮点转定点的类型均为Rx(除了Rx还能是啥？肯定得是Rx啊)
//      定点数转Int32 Rx_toInt32()，事实上只在SVPWM里面使用过一次，由于Timer计数值都是整数的，也就没必要使用cvtRToFloat转成浮点了，直接移位变成非定点数的整数(注意这里不是浮点，而是整数，其实相当于(int32_t)cvtRToFloat())
//      定点数比较Rx_equal()Rx_less()Rx_less_equal()Rx_greater()Rx_greater_equal()非常简单，看函数名也就知道功能了，不再赘述
//      定点数浮点数比较，就是上面的函数名加了个float，一般来说还是得输入constexpr的float，不然依旧会卡成蜗牛！
//
//

// --------------------------------------命名规则--------------------------------------------
#define GenerateRType(x) typedef struct {\
int32_t value;\
}R##x##_t;

GenerateRType(0);GenerateRType(1);GenerateRType(2);GenerateRType(3);
GenerateRType(4);GenerateRType(5);GenerateRType(6);GenerateRType(7);
GenerateRType(8);GenerateRType(9);GenerateRType(10);GenerateRType(11);
GenerateRType(12);GenerateRType(13);GenerateRType(14);GenerateRType(15);
GenerateRType(16);GenerateRType(17);GenerateRType(18);GenerateRType(19);
GenerateRType(20);GenerateRType(21);GenerateRType(22);GenerateRType(23);
GenerateRType(24);GenerateRType(25);GenerateRType(26);GenerateRType(27);
GenerateRType(28);GenerateRType(29);GenerateRType(30);GenerateRType(31);

//将Rx类型转换为float值(使用_Generic宏模拟函数重载，C11特性)
#define cvtRToFloat(r) _Generic((r), \
R0_t:  (r).value * 1.0f / (1U << (31 - 0)), \
R1_t:  (r).value * 1.0f / (1U << (31 - 1)), \
R2_t:  (r).value * 1.0f / (1U << (31 - 2)), \
R3_t:  (r).value * 1.0f / (1U << (31 - 3)), \
R4_t:  (r).value * 1.0f / (1U << (31 - 4)), \
R5_t:  (r).value * 1.0f / (1U << (31 - 5)), \
R6_t:  (r).value * 1.0f / (1U << (31 - 6)), \
R7_t:  (r).value * 1.0f / (1U << (31 - 7)), \
R8_t:  (r).value * 1.0f / (1U << (31 - 8)), \
R9_t:  (r).value * 1.0f / (1U << (31 - 9)), \
R10_t: (r).value * 1.0f / (1U << (31 - 10)), \
R11_t: (r).value * 1.0f / (1U << (31 - 11)), \
R12_t: (r).value * 1.0f / (1U << (31 - 12)), \
R13_t: (r).value * 1.0f / (1U << (31 - 13)), \
R14_t: (r).value * 1.0f / (1U << (31 - 14)), \
R15_t: (r).value * 1.0f / (1U << (31 - 15)), \
R16_t: (r).value * 1.0f / (1U << (31 - 16)), \
R17_t: (r).value * 1.0f / (1U << (31 - 17)), \
R18_t: (r).value * 1.0f / (1U << (31 - 18)), \
R19_t: (r).value * 1.0f / (1U << (31 - 19)), \
R20_t: (r).value * 1.0f / (1U << (31 - 20)), \
R21_t: (r).value * 1.0f / (1U << (31 - 21)), \
R22_t: (r).value * 1.0f / (1U << (31 - 22)), \
R23_t: (r).value * 1.0f / (1U << (31 - 23)), \
R24_t: (r).value * 1.0f / (1U << (31 - 24)), \
R25_t: (r).value * 1.0f / (1U << (31 - 25)), \
R26_t: (r).value * 1.0f / (1U << (31 - 26)), \
R27_t: (r).value * 1.0f / (1U << (31 - 27)), \
R28_t: (r).value * 1.0f / (1U << (31 - 28)), \
R29_t: (r).value * 1.0f / (1U << (31 - 29)), \
R30_t: (r).value * 1.0f / (1U << (31 - 30)), \
R31_t: (r).value * 1.0f / (1U << (31 - 31)) \
)
//牛顿法对u32开方
static inline uint32_t sqrt_newton_u32(uint32_t n) {
    if (n == 0) return 0;
    if (n <= 3) return 1;

    // 初始值选择：避免从 n 开始（加速收敛）
    uint32_t x = n;
    if (n > 0xFFFFU) x = 0x10000U;    // 2^16（适用于 n 接近 2^32）
    else if (n > 0xFFU) x = 0x100U;   // 2^8
    else x = 0x10U;                    // 2^4

    uint32_t y = (x + n / x) >> 1;
    while (y < x) {
        x = y;
        y = (x + n / x) >> 1;
    }
    return x;
}
//位移法对u32开方
static inline uint32_t sqrt_bitwise_u32(uint32_t n) {
    if (n == 0) return 0;

    uint32_t root = 0;
    uint32_t bit = 1U << 30;  // (2^15)^2 = 2^30，因为 (2^16)^2 = 2^32 超出 uint32 范围

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
//上述两函数由ai生成，已经过随机验证
/*
// 生成 [0, UINT32_MAX] 的随机 uint32_t
uint32_t rand_u32(void) {
    return  (uint32_t)rand();
}

int main(void) {
    // 初始化随机种子
    srand((unsigned)time(NULL));


    for (int i=0;i<10;++i) {
        uint32_t n = rand_u32();
        uint32_t ref = (uint32_t)sqrt((double)n);
        uint32_t yours = sqrt_newton_u32(n);

        printf("测试值: %"PRIu32",标准函数输出:%u,测试函数输出:%u,\n", n,ref,yours);
    }
}
 */

// 专用于R4的转换宏
# define R4_fromFloat(x) ((R4_t){ (x) *(1U<<(31-4)) })

//定点数浮点数指定定点精度乘法，相当于x*y
static inline R2_t R2_f2_2_mul(R2_t x,float y) {
    return (R2_t){(int32_t)(((int64_t)x.value*((int32_t)(y*(float)(1U<<(31-2)))))>>(31+2-(2+2)))};
}
//定点数加法，相当于x+y
static inline R2_t R2_add(R2_t x,R2_t y) {
    return (R2_t){x.value+y.value};
}
//定点数浮点数指定定点精度乘法，相当于x*y
static inline R2_t R2_f0_2_mul(R2_t x,float y) {
    return (R2_t){(int32_t)(((int64_t)x.value*((int32_t)(y*(float)(1U<<(31-0)))))>>(31+2-(2+0)))};
}

//定点数浮点数指定定点精度乘法，相当于x*y
static inline R4_t R4_f1_4_mul(R4_t x,float y) {
    return (R4_t){(int32_t)(((int64_t)x.value*((int32_t)(y*(float)(1U<<(31-1)))))>>(31+4-(4+1)))};
}


//定点数减法，相当于x-y
static inline R4_t R4_sub(R4_t x,R4_t y) {
    return (R4_t){x.value-y.value};
}
//定点数浮点数指定定点精度乘法，相当于x*y
static inline R4_t R4_f0_4_mul(R4_t x,float y) {
    return (R4_t){(int32_t)(((int64_t)x.value*((int32_t)(y*(float)(1U<<(31-0)))))>>(31+4-(4+0)))};
}

//定点数取反，相当于-x
static inline R4_t R4_neg(R4_t x) {
    x.value=-x.value;
    return x;
}
//定点数加法，相当于x+y
static inline R4_t R4_add(R4_t x,R4_t y) {
    return (R4_t){x.value+y.value};
}
//定点数乘法，相当于x*y
static inline R2_t R2_0_2_mul(R2_t x,R0_t y) {
    return (R2_t){(int32_t)(((int64_t)x.value*y.value)>>(31+2-(2+0)))};
}

//定点数减法，相当于x-y
static inline R2_t R2_sub(R2_t x,R2_t y) {
    return (R2_t){x.value-y.value};
}

//定点数乘法，相当于x*y
static inline R4_t R4_0_4_mul(R4_t x,R0_t y) {
    return (R4_t){(int32_t)(((int64_t)x.value*y.value)>>(31+4-(4+0)))};
}

// 专用于R0的转换宏
# define R0_fromFloat(x) ((R0_t){ (x) *(1U<<(31-0)) })
// 专用于R15的转换宏
# define R15_fromFloat(x) ((R15_t){ (x) *(1U<<(31-15)) })
// 专用于R8的转换宏
# define R8_fromFloat(x) ((R8_t){ (x) *(1U<<(31-8)) })
// 专用于R5的转换宏
# define R5_fromFloat(x) ((R5_t){ (x) *(1U<<(31-5)) })

//定点数浮点数判断相等，相当于x==y
static inline int32_t R4_float_equal(R4_t x,float y) {
    return x.value==(int32_t)(y*(float)(1U<<(31-(4))));
}
//定点数浮点数判断小于，相当于x<y
static inline int32_t R4_float_less(R4_t x,float y) {
    return x.value<(int32_t)(y*(float)(1U<<(31-(4))));
}
//定点数浮点数判断小于等于，相当于x<=y
static inline int32_t R4_float_less_equal(R4_t x,float y) {
    return x.value<=(int32_t)(y*(float)(1U<<(31-(4))));
}
//定点数浮点数判断大于，相当于x>y
static inline int32_t R4_float_greater(R4_t x,float y) {
    return x.value>(int32_t)(y*(float)(1U<<(31-(4))));
}
//定点数浮点数判断大于等于，相当于x>=y
static inline int32_t R4_float_greater_equal(R4_t x,float y) {
    return x.value>=(int32_t)(y*(float)(1U<<(31-(4))));
}

//定点数浮点数指定定点精度乘法，相当于x*y
static inline R15_t R0_f15_15_mul(R0_t x,float y) {
    return (R15_t){(int32_t)(((int64_t)x.value*((int32_t)(y*(float)(1U<<(31-15)))))>>(31+15-(0+15)))};
}
//定点数乘法，相当于x*y
static inline R15_t R15_4_15_mul(R15_t x,R4_t y) {
    return (R15_t){(int32_t)(((int64_t)x.value*y.value)>>(31+15-(15+4)))};
}
//定点数取反，相当于-x
static inline R15_t R15_neg(R15_t x) {
    x.value=-x.value;
    return x;
}

//定点数转整数，事实上该函数只用于SVPWM
static inline int32_t R15_toInt32(R15_t x) {
    return x.value>>(31-15);
}

//定点数加法，相当于x+y+z
static inline R2_t R2_add_3(R2_t x,R2_t y,R2_t z) {
    return (R2_t){x.value+y.value+z.value};
}

//定点数乘法，相当于x*y
static inline R15_t R15_0_15_mul(R15_t x,R0_t y) {
    return (R15_t){(int32_t)(((int64_t)x.value*y.value)>>(31+15-(15+0)))};
}

//定点数浮点数指定定点精度乘法，相当于x*y
static inline R2_t R15_f0_2_mul(R15_t x,float y) {
    return (R2_t){(int32_t)(((int64_t)x.value*((int32_t)(y*(float)(1U<<(31-0)))))>>(31+2-(15+0)))};
}

//定点数浮点数指定定点精度乘法，相当于x*y
static inline R2_t R4_f0_2_mul(R4_t x,float y) {
    return (R2_t){(int32_t)(((int64_t)x.value*((int32_t)(y*(float)(1U<<(31-0)))))>>(31+2-(4+0)))};
}

//定点数浮点数指定定点精度乘法，相当于x*y
static inline R4_t R15_f4_4_mul(R15_t x,float y) {
    return (R4_t){(int32_t)(((int64_t)x.value*((int32_t)(y*(float)(1U<<(31-4)))))>>(31+4-(15+4)))};
}

//定点数浮点数指定定点精度乘法，相当于x*y
static inline R0_t R0_f0_0_mul(R0_t x,float y) {
    return (R0_t){(int32_t)(((int64_t)x.value*((int32_t)(y*(float)(1U<<(31-0)))))>>(31+0-(0+0)))};
}
//定点数乘法，相当于x*y
static inline R0_t R15_0_0_mul(R15_t x,R0_t y) {
    return (R0_t){(int32_t)(((int64_t)x.value*y.value)>>(31+0-(15+0)))};
}

//定点数加法，相当于x+y+z+u+v+w
static inline R0_t R0_add_6(R0_t x,R0_t y,R0_t z,R0_t u,R0_t v,R0_t w) {
    return (R0_t){x.value+y.value+z.value+u.value+v.value+w.value};
}
//定点数浮点数指定定点精度乘法，相当于x*y
static inline R0_t R0_f1_0_mul(R0_t x,float y) {
    return (R0_t){(int32_t)(((int64_t)x.value*((int32_t)(y*(float)(1U<<(31-1)))))>>(31+0-(0+1)))};
}
//定点数加法，相当于x+y
static inline R0_t R0_add(R0_t x,R0_t y) {
    return (R0_t){x.value+y.value};
}
//定点数乘法，相当于x*y
static inline R0_t R8_0_0_mul(R8_t x,R0_t y) {
    return (R0_t){(int32_t)(((int64_t)x.value*y.value)>>(31+0-(8+0)))};
}
//定点数乘法，相当于x*y
static inline R0_t R0_0_0_mul(R0_t x,R0_t y) {
    return (R0_t){(int32_t)(((int64_t)x.value*y.value)>>(31+0-(0+0)))};
}
//定点数乘法，相当于x*y
static inline R2_t R8_0_2_mul(R8_t x,R0_t y) {
    return (R2_t){(int32_t)(((int64_t)x.value*y.value)>>(31+2-(8+0)))};
}
//定点数乘法，相当于x*y
static inline R0_t R2_0_0_mul(R2_t x,R0_t y) {
    return (R0_t){(int32_t)(((int64_t)x.value*y.value)>>(31+0-(2+0)))};
}
//定点数浮点数指定定点精度乘法，相当于x*y
static inline R0_t R0_f2_0_mul(R0_t x,float y) {
    return (R0_t){(int32_t)(((int64_t)x.value*((int32_t)(y*(float)(1U<<(31-2)))))>>(31+0-(0+2)))};
}
// 专用于R2的转换宏
# define R2_fromFloat(x) ((R2_t){ (x) *(1U<<(31-2)) })
//定点数加法，相当于x+y+z+u+v
static inline R0_t R0_add_5(R0_t x,R0_t y,R0_t z,R0_t u,R0_t v) {
    return (R0_t){x.value+y.value+z.value+u.value+v.value};
}

//定点数乘法，相当于x*y
static inline R2_t R15_0_2_mul(R15_t x,R0_t y) {
    return (R2_t){(int32_t)(((int64_t)x.value*y.value)>>(31+2-(15+0)))};
}

//定点数加法，相当于x+y
static inline R15_t R15_add(R15_t x,R15_t y) {
    return (R15_t){x.value+y.value};
}

//定点数加法，相当于x+y+z+u
static inline R2_t R2_add_4(R2_t x,R2_t y,R2_t z,R2_t u) {
    return (R2_t){x.value+y.value+z.value+u.value};
}
//定点数浮点数指定定点精度乘法，相当于x*y
static inline R2_t R8_f0_2_mul(R8_t x,float y) {
    return (R2_t){(int32_t)(((int64_t)x.value*((int32_t)(y*(float)(1U<<(31-0)))))>>(31+2-(8+0)))};
}
//定点数加法，相当于x+y
static inline R8_t R8_add(R8_t x,R8_t y) {
    return (R8_t){x.value+y.value};
}
//定点数浮点数指定定点精度乘法，相当于x*y
static inline R8_t R15_f0_8_mul(R15_t x,float y) {
    return (R8_t){(int32_t)(((int64_t)x.value*((int32_t)(y*(float)(1U<<(31-0)))))>>(31+8-(15+0)))};
}

//定点数加法，相当于x+y+z
static inline R8_t R8_add_3(R8_t x,R8_t y,R8_t z) {
    return (R8_t){x.value+y.value+z.value};
}
//定点数浮点数指定定点精度乘法，相当于x*y
static inline R8_t R8_f0_8_mul(R8_t x,float y) {
    return (R8_t){(int32_t)(((int64_t)x.value*((int32_t)(y*(float)(1U<<(31-0)))))>>(31+8-(8+0)))};
}
//定点数乘法，相当于x*y
static inline R8_t R15_0_8_mul(R15_t x,R0_t y) {
    return (R8_t){(int32_t)(((int64_t)x.value*y.value)>>(31+8-(15+0)))};
}

//定点数乘法，相当于x*y
static inline R8_t R8_0_8_mul(R8_t x,R0_t y) {
    return (R8_t){(int32_t)(((int64_t)x.value*y.value)>>(31+8-(8+0)))};
}

//定点数加法，相当于x+y+z
static inline R0_t R0_add_3(R0_t x,R0_t y,R0_t z) {
    return (R0_t){x.value+y.value+z.value};
}
//定点数浮点数指定定点精度乘法，相当于x*y
static inline R0_t R8_f0_0_mul(R8_t x,float y) {
    return (R0_t){(int32_t)(((int64_t)x.value*((int32_t)(y*(float)(1U<<(31-0)))))>>(31+0-(8+0)))};
}
//定点数减法，相当于x-y
static inline R0_t R0_sub(R0_t x,R0_t y) {
    return (R0_t){x.value-y.value};
}

// R0取倒数转R8
static inline R8_t R0_inv(R0_t x) {
    if (x.value==0)return (R8_t){0};
    return (R8_t){(INT32_MAX/(x.value>>16))<<(15-8)};
}

//定点数加法，相当于x+y+z
static inline R15_t R15_add_3(R15_t x,R15_t y,R15_t z) {
    return (R15_t){x.value+y.value+z.value};
}
//定点数乘法，相当于x*y
static inline R15_t R8_2_15_mul(R8_t x,R2_t y) {
    return (R15_t){(int32_t)(((int64_t)x.value*y.value)>>(31+15-(8+2)))};
}
//定点数加法，相当于x+y+z
static inline R4_t R4_add_3(R4_t x,R4_t y,R4_t z) {
    return (R4_t){x.value+y.value+z.value};
}
//定点数乘法，相当于x*y
static inline R4_t R2_0_4_mul(R2_t x,R0_t y) {
    return (R4_t){(int32_t)(((int64_t)x.value*y.value)>>(31+4-(2+0)))};
}

//定点数判断相等，相当于x==y
static inline int32_t R4_equal(R4_t x,R4_t y) {
    return x.value==y.value;
}
//定点数判断小于，相当于x<y
static inline int32_t R4_less(R4_t x,R4_t y) {
    return x.value<y.value;
}
//定点数判断小于等于，相当于x<=y
static inline int32_t R4_less_equal(R4_t x,R4_t y) {
    return x.value<=y.value;
}
//定点数判断大于，相当于x>y
static inline int32_t R4_greater(R4_t x,R4_t y) {
    return x.value>y.value;
}
//定点数判断大于等于，相当于x>=y
static inline int32_t R4_greater_equal(R4_t x,R4_t y) {
    return x.value>=y.value;
}
//定点数减法，相当于x-y-z
static inline R0_t R0_sub_3(R0_t x,R0_t y,R0_t z) {
    return (R0_t){x.value-y.value-z.value};
}
//定点数减法，相当于x-y-z
static inline R8_t R8_sub_3(R8_t x,R8_t y,R8_t z) {
    return (R8_t){x.value-y.value-z.value};
}
//定点数减法，相当于x-y-z
static inline R15_t R15_sub_3(R15_t x,R15_t y,R15_t z) {
    return (R15_t){x.value-y.value-z.value};
}
//定点数乘法，相当于x*y
static inline R15_t R8_8_15_mul(R8_t x,R8_t y) {
    return (R15_t){(int32_t)(((int64_t)x.value*y.value)>>(31+15-(8+8)))};
}
//定点数减法，相当于x-y-z
static inline R2_t R2_sub_3(R2_t x,R2_t y,R2_t z) {
    return (R2_t){x.value-y.value-z.value};
}
//定点数乘法，相当于x*y
static inline R2_t R0_0_2_mul(R0_t x,R0_t y) {
    return (R2_t){(int32_t)(((int64_t)x.value*y.value)>>(31+2-(0+0)))};
}

//定点数乘法，相当于x*y
static inline R4_t R4_2_4_mul(R4_t x,R2_t y) {
    return (R4_t){(int32_t)(((int64_t)x.value*y.value)>>(31+4-(4+2)))};
}
//定点数乘法，相当于x*y
static inline R4_t R0_2_4_mul(R0_t x,R2_t y) {
    return (R4_t){(int32_t)(((int64_t)x.value*y.value)>>(31+4-(0+2)))};
}
//定点数减法，相当于x-y
static inline R15_t R15_sub(R15_t x,R15_t y) {
    return (R15_t){x.value-y.value};
}
//定点数乘法，相当于x*y
static inline R2_t R0_15_2_mul(R0_t x,R15_t y) {
    return (R2_t){(int32_t)(((int64_t)x.value*y.value)>>(31+2-(0+15)))};
}
//定点数判断相等，相当于x==y
static inline int32_t R2_equal(R2_t x,R2_t y) {
    return x.value==y.value;
}
//定点数判断小于，相当于x<y
static inline int32_t R2_less(R2_t x,R2_t y) {
    return x.value<y.value;
}
//定点数判断小于等于，相当于x<=y
static inline int32_t R2_less_equal(R2_t x,R2_t y) {
    return x.value<=y.value;
}
//定点数判断大于，相当于x>y
static inline int32_t R2_greater(R2_t x,R2_t y) {
    return x.value>y.value;
}
//定点数判断大于等于，相当于x>=y
static inline int32_t R2_greater_equal(R2_t x,R2_t y) {
    return x.value>=y.value;
}
//定点数乘法，相当于x*y
static inline R4_t R0_10_4_mul(R0_t x,R10_t y) {
    return (R4_t){(int32_t)(((int64_t)x.value*y.value)>>(31+4-(0+10)))};
}
//定点数乘法，相当于x*y
static inline R2_t R0_22_2_mul(R0_t x,R22_t y) {
    return (R2_t){(int32_t)(((int64_t)x.value*y.value)>>(31+2-(0+22)))};
}
//定点数乘法，相当于x*y
static inline R2_t R0_18_2_mul(R0_t x,R18_t y) {
    return (R2_t){(int32_t)(((int64_t)x.value*y.value)>>(31+2-(0+18)))};
}
//定点数取反，相当于-x
static inline R2_t R2_neg(R2_t x) {
    x.value=-x.value;
    return x;
}

//定点数乘法，相当于x*y
static inline R8_t R4_4_8_mul(R4_t x,R4_t y) {
    return (R8_t){(int32_t)(((int64_t)x.value*y.value)>>(31+8-(4+4)))};
}
static inline R8_t R8_sqrt(R8_t x) {
    if (x.value<0)return (R8_t){0};
    return (R8_t){sqrt_newton_u32(x.value>>1)<<((32-8)/2)};
}

//定点数浮点数判断大于，相当于x>y
static inline int32_t R8_float_greater(R8_t x,float y) {
    return x.value>(int32_t)(y*(float)(1U<<(31-(8))));
}
// R8取倒数转R0
static inline R0_t R8_inv(R8_t x) {
    if (x.value==0)return (R0_t){0};
    return (R0_t){(INT32_MAX/(x.value>>16))<<(15-8)};
}
//定点数浮点数指定定点精度乘法，相当于x*y
static inline R0_t R0_f4_0_mul(R0_t x,float y) {
    return (R0_t){(int32_t)(((int64_t)x.value*((int32_t)(y*(float)(1U<<(31-4)))))>>(31+0-(0+4)))};
}

#endif //FOC_SENSORLESS_FIXED_FIX_POINT_H