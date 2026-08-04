//
// Created by lxcy on 2026/7/27.
//

#ifndef FOC_SENSORLESS_FIXED_CLARKE_PARK_H
#define FOC_SENSORLESS_FIXED_CLARKE_PARK_H
#include "fixed_point.h"
#include "User_Parameters.h"

typedef struct Clarke_Park_t{
    struct Clarke_t{
        Q15_I_t Ia_I;
        Q15_I_t Ib_I;
        Q15_I_t Ic_I;//无用
        Q15_I_t Ialpha_O;
        Q15_I_t Ibeta_O;
    }clarke;
    struct IClarke_t {
        Q15_U_t Valpha_I;
        Q15_U_t Vbeta_I;
        Q15_U_t Va_O;
        Q15_U_t Vb_O;
        Q15_U_t Vc_O;
    }iclarke;
    struct Park_t {
        Q15_I_t Ialpha_I;
        Q15_I_t Ibeta_I;
        Q15_te_t Theta_I;//弧度制
        Q15_I_t Id_O;
        Q15_I_t Iq_O;
    }park;
    struct IPark_t {
        Q15_U_t Vd_I;
        Q15_U_t Vq_I;
        Q15_te_t Theta_I;
        Q15_U_t Valpha_O;
        Q15_U_t Vbeta_O;
    }ipark;
};
extern struct Clarke_Park_t clarke_park_transform;
#define ClarkePark clarke_park_transform //简短的变量名定义

void Clarke_transform(struct Clarke_t* clarke);
void IClarke_transform(struct IClarke_t* iclarke);
void Park_transform(struct Park_t* park);
void IPark_transform(struct IPark_t* ipark);
#endif //FOC_SENSORLESS_FIXED_CLARKE_PARK_H
