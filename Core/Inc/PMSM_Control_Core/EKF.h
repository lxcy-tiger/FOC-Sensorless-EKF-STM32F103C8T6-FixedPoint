//
// Created by lxcy on 2025/11/30.
//

#ifndef FOC_SENSORLESS_FIXED_EKF_H
#define FOC_SENSORLESS_FIXED_EKF_H

#include "other.h"
struct EKF_t {
    R4_t Valpha_I;//输入Valpha(V)
    R4_t Vbeta_I;//输入Vbeta(V)
    R2_t Ialpha_I;//输入Ialpha(A)
    R2_t Ibeta_I;//输入Ibeta(A)

    R2_t Ialpha_O;//输出预测值Ialpha(A)
    R2_t Ibeta_O;//输出预测值Ibeta(A)
    R15_t Espeed_O;//输出电角速度Espeed(rad/s)
    R4_t Etheta_O;//输出电角度Etheta(rad)
};
extern struct EKF_t ekf_est;
void EKF_update(struct EKF_t*ekf);//更新EKF

#endif //FOC_SENSORLESS_FIXED_EKF_H