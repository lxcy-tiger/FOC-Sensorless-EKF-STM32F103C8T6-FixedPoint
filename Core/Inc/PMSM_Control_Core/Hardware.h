//
// Created by lxcy on 2025/11/30.
//

#ifndef FOC_SENSORLESS_FIXED_HARDWARE_H
#define FOC_SENSORLESS_FIXED_HARDWARE_H
#include "fixed_point.h"
#include "main.h"
extern R15_t VCC_3V3_R; //稳压模块输出电压
extern R15_t IA_REF_R;  //Ia偏置电压ADC值(12位ADC),离线校正
extern R15_t IB_REF_R; //Ib偏置电压ADC值(12位ADC),离线校正
constexpr float IA_K=0.33f*2200/(680+2200)*2;//增益，由电阻网络计算得到(取正值)，单位V/A
constexpr float IB_K=0.33f*2200/(680+2200)*2;//增益，由电阻网络计算得到(取正值)，单位V/A
constexpr R0_t IA_K_4095_inv=R0_fromFloat(1/IA_K/4095.f);//乘以它相当于除以IA_K再除以4095
constexpr R0_t IB_K_4095_inv=R0_fromFloat(1/IB_K/4095.f);//乘以它相当于除以IB_K再除以4095
extern R0_t IA_K_4095_inv_VCC_3V3;//乘以它相当于乘以3V3除以IA_K再除以4095
extern R0_t IB_K_4095_inv_VCC_3V3;//乘以它相当于乘以3V3除以IB_K再除以4095
constexpr R0_t IA_K_R=R0_fromFloat(IA_K);//增益，由电阻网络计算得到(取正值)，单位V/A
constexpr R0_t IB_K_R=R0_fromFloat(IB_K);//增益，由电阻网络计算得到(取正值)，单位V/A
static constexpr uint8_t POLE_PAIRS=7; //极对数
void Offline_IabVcc_Adjust();
#endif //FOC_SENSORLESS_FIXED_HARDWARE_H