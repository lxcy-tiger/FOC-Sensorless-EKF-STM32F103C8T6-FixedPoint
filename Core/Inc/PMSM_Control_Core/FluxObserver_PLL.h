//
// Created by lxcy on 2025/12/19.
//

#ifndef FOC_SENSORLESS_FIXED_FLUXOBSERVER_PLL_H
#define FOC_SENSORLESS_FIXED_FLUXOBSERVER_PLL_H
#include "other.h"
struct FluxObserver_PLL_t {
    R4_t Valpha_I;//输入Valpha(V)
    R4_t Vbeta_I;//输入Vbeta(V)
    R2_t Ialpha_I;//输入Ialpha(A)
    R2_t Ibeta_I;//输入Ibeta(A)

    R0_t Flux_alpha_O;//输出转子磁链Flux_alpha(V·s)
    R0_t Flux_beta_O;//输出转子磁链Flux_beta(V·s)
    R15_t Espeed_O;//输出电角速度Espeed(rad/s)
    R4_t Etheta_O;//输出电角度Etheta(rad)
};
extern struct FluxObserver_PLL_t fluxObserver_pll_est;

void FluxObserver_PLL_update(struct FluxObserver_PLL_t*fluxObserver_pll_est);

#endif //FOC_SENSORLESS_FIXED_FLUXOBSERVER_PLL_H