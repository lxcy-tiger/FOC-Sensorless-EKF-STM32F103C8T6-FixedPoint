//
// Created by lxcy on 2026/7/28.
//

#ifndef FOC_SENSORLESS_FIXED_HARDWARE_H
#define FOC_SENSORLESS_FIXED_HARDWARE_H
#include "PMSM_Control_Core/User_Parameters.h"
//校正VCC_3V3,IA_REF,IB_REF
void Offline_IabVcc_Adjust();
//将ADC采样值转换为Q15表示的电流值
Q15_I_t ADC_GetCurrent_Q15(uint16_t I_adc,int16_t I_ref);
#endif //FOC_SENSORLESS_FIXED_HARDWARE_H
