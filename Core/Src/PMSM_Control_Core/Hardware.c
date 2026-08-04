//
// Created by lxcy on 2026/7/28.
//
#include "PMSM_Control_Core/Hardware.h"
#include "adc.h"
static const uint32_t VREFINT_CAL_ADDR = 1438; //测得在3.3V时VREFINT的值为1438
float VCC_3V3=3.314f; //核心板VCC电压实测
int16_t IA_REF=4095*680/(680+2200)*2; //Ia偏置电压ADC值(12位ADC),根据电阻网络计算得到
int16_t IB_REF=4095*680/(680+2200)*2; //Ib偏置电压ADC值(12位ADC),根据电阻网络计算得到
static const float I_K=0.33*2200/(680+2200)*2;//增益，由电阻网络计算得到(取正值)，单位V/A

/*获取实际电流的步骤：
 * (ADC测量值-ADC偏置值)*VCC_3V3/4095.f === V0 为采样电阻的电压放大后的实际电压值
 * -V0/I_K === IA 为A相电流(以流入电机为正方向)
 * 但是由于要定点计算，因此计算为:
 * (ADC测量值_int16_t-ADC偏置值_int16_t)*(-VCC_3V3/4095.f/I_K)=IA_float
 * (IA_float/I_q15base.base_value*32768)=IA_Q15_t
 * 或者说(ADC测量值_int16_t-ADC偏置值_int16_t)*(-VCC_3V3/4095.f/I_K/I_q15base.base_value*32768)=IA_Q15_t
 * 后面ADC_Current_To_Q15_float=-VCC_3V3/4095.f/I_K/I_q15base.base_value*32768的部分可以改成定点形式
 * 其典型值是ADC_Current_To_Q15_float≈-3.3/4095/0.5/1.1*32768=-48.f
 * 参考Q15_gain函数的设计，由于在编译时，该值是不确定的，因此其不能直接调用Q15_gain，否则会引入浮点运算
 * 由于我们知道 ADC_raw=(ADC测量值_int16_t-ADC偏置值_int16_t)的范围在[-4095,4095]即12位数值，因而为保证精度，可以设计成为32位定点运算
 * IA_Q15_t = ADC_raw * (ADC_Current_To_Q15_float * (1<<(31-x))) >> (31-x)
 * x可以根据ADC_Current_To_Q15_float的实际大小动态选择，如上面计算得到ADC_Current_To_Q15_float=48<2^6
 * 并且|ADC_raw|<4096=2^12
 * 所以x可以取6+12=18防止计算结果溢出
 * 即ADC_Current_To_Q15=ADC_Current_To_Q15_float* (1<<(31-18))=-48*(1<<(31-18))=-393216
*/
static int32_t ADC_Current_To_Q15;//乘以它再右移(31-x)相当于乘以(-VCC_3V3/4095.f/I_K/I_q15base.base_value*32768)
static int32_t ADC_Current_To_Q15_x;
//校正VCC_3V3,IA_REF,IB_REF
void Offline_IabVcc_Adjust() {
    uint32_t IarefAddUp=0;
    uint32_t IbrefAddUp=0;
    uint32_t VrefAddUp=0;
    const uint16_t Repeat=10000;
    //12位的数据，乘以16位计数，小于32位的数据(4095*10000<U32_MAX)
    for (uint16_t i=0;i<Repeat;i++) {
        uint16_t ADCres[3];
        HAL_ADC_Start_DMA(&hadc1,(uint32_t*)ADCres,3);
        while (HAL_DMA_GetState(hadc1.DMA_Handle) == HAL_DMA_STATE_BUSY);
        HAL_ADC_Stop_DMA(&hadc1);
        IarefAddUp+=ADCres[0];
        IbrefAddUp+=ADCres[1];
        VrefAddUp+=ADCres[2];
    }
    IA_REF=IarefAddUp/Repeat;
    IB_REF=IbrefAddUp/Repeat;
    VCC_3V3=(VREFINT_CAL_ADDR)*3.3f*(float)Repeat/(float)VrefAddUp;

    float ADC_Current_To_Q15_float=-VCC_3V3/4095.f/I_K/I_q15base.base_value*32768;
    for (ADC_Current_To_Q15_x=0;
        my_abs(ADC_Current_To_Q15_float)>=(1<<ADC_Current_To_Q15_x) && ADC_Current_To_Q15_x < (31-12);
        ADC_Current_To_Q15_x++);

    ADC_Current_To_Q15_x+=12;

    ADC_Current_To_Q15=ADC_Current_To_Q15_float*(1<<(31-ADC_Current_To_Q15_x));
}
//将ADC采样值转换为Q15表示的电流值
Q15_I_t ADC_GetCurrent_Q15(uint16_t I_adc,int16_t I_ref) {
    return (Q15_I_t){(I_adc-I_ref)*ADC_Current_To_Q15>>(31-ADC_Current_To_Q15_x)};
}