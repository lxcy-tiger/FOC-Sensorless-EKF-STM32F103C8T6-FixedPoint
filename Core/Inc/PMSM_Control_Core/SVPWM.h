//
// Created by lxcy on 2025/11/30.
//

#ifndef FOC_SENSORLESS_FIXED_SVPWM_H
#define FOC_SENSORLESS_FIXED_SVPWM_H

#include "main.h"
#include "tim.h"
#include "fixed_point.h"
#include "other.h"
//更新Udc和DivUdc唯一接口
void SetUdc(float newUdc);
//获取Udc唯一接口
float GetUdc();
//设定PWM比较值，这里ABC三相分别对应TIMER1三个输出通道。(-O2自动内联)
static void TIM1_Set_PWMCompare(uint16_t A,uint16_t B,uint16_t C) {
    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1, A);
    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, B);
    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_3, C);
}
//计算SVPWM并更新到HRTIM的计数值(内部实现简单非线性限幅，建议在外部对Valpha和Vbeta进行等比例限幅，以避免过调制)
void SVPWM_Calculate_Set(R4_t Valpha, R4_t Vbeta);
#endif //FOC_SENSORLESS_FIXED_SVPWM_H