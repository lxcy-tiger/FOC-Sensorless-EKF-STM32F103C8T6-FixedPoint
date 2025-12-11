//
// Created by lxcy on 2025/11/30.
//
#include "PMSM_Control_Core/Hardware.h"

#include "adc.h"
constexpr uint32_t VREFINT_CAL_ADDR = 1438; //测得在3.3V时VREFINT的值为1438
float VCC_3V3=3.314f; //核心板VCC电压实测
uint16_t IA_REF=4095*680/(680+2200)*2; //Ia偏置电压ADC值(12位ADC),根据电阻网络计算得到
uint16_t IB_REF=4095*680/(680+2200)*2; //Ib偏置电压ADC值(12位ADC),根据电阻网络计算得到
R15_t VCC_3V3_R; //稳压模块输出电压
R15_t IA_REF_R;  //Ia偏置电压ADC值(12位ADC),离线校正
R15_t IB_REF_R; //Ib偏置电压ADC值(12位ADC),离线校正
R0_t IA_K_4095_inv_VCC_3V3;//乘以它相当于乘以3V3除以IA_K再除以4095
R0_t IB_K_4095_inv_VCC_3V3;//乘以它相当于乘以3V3除以IB_K再除以4095
/*获取实际电流的步骤：
 * (ADC测量值-ADC偏置值)*VCC_3V3/4095.f === V0 为采样电阻电压
 * -V0/IA_K === IA 为A相电压(以流入电机为正方向)
*/

//校正VCC_3V3,IA_REF,IB_REF
void Offline_IabVcc_Adjust() {
    uint32_t IarefAddUp=0;
    uint32_t IbrefAddUp=0;
    uint32_t VrefAddUp=0;
    constexpr uint16_t Repeat=10000;
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

    IA_REF_R=R15_fromFloat(IA_REF);
    IB_REF_R=R15_fromFloat(IB_REF);
    VCC_3V3_R=R15_fromFloat(VCC_3V3);
    IA_K_4095_inv_VCC_3V3=R0_fromFloat(VCC_3V3/IA_K/4095.f);
    IB_K_4095_inv_VCC_3V3=R0_fromFloat(VCC_3V3/IB_K/4095.f);
}
