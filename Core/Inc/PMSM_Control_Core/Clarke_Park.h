//
// Created by lxcy on 2025/11/30.
//

#ifndef FOC_SENSORLESS_FIXED_CLARKE_PARK_H
#define FOC_SENSORLESS_FIXED_CLARKE_PARK_H
#include "other.h"
struct Clarke_Park_t{
    struct Clarke_t{
        R2_t Ia_I;
        R2_t Ib_I;
        R2_t Ic_I;//无用
        R2_t Ialpha_O;
        R2_t Ibeta_O;
    }clarke;
    struct IClarke_t {
        R4_t Valpha_I;
        R4_t Vbeta_I;
        R4_t Va_O;
        R4_t Vb_O;
        R4_t Vc_O;
    }iclarke;
    struct Park_t {
        R2_t Ialpha_I;
        R2_t Ibeta_I;
        R4_t Theta_I;//弧度制
        R2_t Id_O;
        R2_t Iq_O;
    }park;
    struct IPark_t {
        R4_t Vd_I;
        R4_t Vq_I;
        R4_t Theta_I;
        R4_t Valpha_O;
        R4_t Vbeta_O;
    }ipark;
};
extern struct Clarke_Park_t clarke_park_transform;
#define ClarkePark clarke_park_transform //简短的变量名定义

void Clarke_transform(struct Clarke_t* clarke);
void IClarke_transform(struct IClarke_t* iclarke);
void Park_transform(struct Park_t* park);
void IPark_transform(struct IPark_t* ipark);

#endif //FOC_SENSORLESS_FIXED_CLARKE_PARK_H