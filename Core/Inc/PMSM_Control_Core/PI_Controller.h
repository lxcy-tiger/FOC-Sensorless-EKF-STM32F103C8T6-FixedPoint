//
// Created by lxcy on 2026/7/14.
//

#ifndef FOC_SENSORLESS_FIXED_PI_CONTROLLER_H
#define FOC_SENSORLESS_FIXED_PI_CONTROLLER_H
#include "fixed_point.h"
#include "User_Parameters.h"

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
    Q15_##Output_QType##_t I_Output_SinglePart;/*输出值的单次积分计算值部分*/                                    \
    Q15_##Output_QType##_t I_Output_Part;/*输出值的积分部分*/                                    \
    Q15_##Output_QType##_t Output;/*输出值*/                                    \
};                                                            \
static inline void NAME##_PI_update(struct PI_Controller_##NAME##_t* NAME){ \
    /*CalculateError为0时，用户给定error，直接用用户给定的error计算，我们不会计算并覆盖掉它*/\
    if(CalculateError)NAME->error =Q15_sub(Q15_C2P(NAME->Set) , Q15_C2P(NAME->Measure)); \
    NAME->P_Output_Part=(Q15_##Output_QType##_t){satInt32(Q15_gain(NAME->error,P_VAL*Input_QType##_q15base.base_value/Output_QType##_q15base.base_value).parent_value)};\
    NAME->I_Output_SinglePart=(Q15_##Output_QType##_t){satInt32(Q15_gain(NAME->error,I_Ts_VAL*Input_QType##_q15base.base_value/Output_QType##_q15base.base_value).parent_value)};\
    Q15_t output_unsat =Q15_aa(_P(NAME->P_Output_Part),_P(NAME->I_Output_SinglePart),_P(NAME->I_Output_Part)); \
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
    NAME->I_Output_Part.child_value += NAME->I_Output_SinglePart.child_value; \
    } \
        NAME->Output = output; \
    } \
    extern struct PI_Controller_##NAME##_t NAME##_PIstate;

#define Current_P_VAL 12.566f
#define Current_I_Ts_VAL 2.30376f
GenerateFunction_PIController(Id,Current_P_VAL,Current_I_Ts_VAL,6.5f,-6.5f,1,I,U);
GenerateFunction_PIController(Iq,Current_P_VAL,Current_I_Ts_VAL,6.5f,-6.5f,1,I,U);
#define Speed_P_VAL 0.008712368f
#define Speed_I_Ts_VAL 2.138e-6f
GenerateFunction_PIController(Speed,Speed_P_VAL, Speed_I_Ts_VAL,1.0f,-1.0f,1,we,I);



GenerateFunction_PIController(FluxObserver_PLLSpeed,64404.56f,150.14f,1700.f,-1700.f,0,flux,we);

GenerateFunction_PIController(SMO_PLLSpeed,1398.38,48.89,3000,-3000,0,te,we);

GenerateFunction_PIController(ST_SMO_PLLSpeed,1398.38,48.89,3000,-3000,0,te,we);

#endif //FOC_SENSORLESS_FIXED_PI_CONTROLLER_H
