//
// Created by lxcy on 2026/7/14.
//

#ifndef FOC_SENSORLESS_FIXED_FLUXOBSERVER_PLL_H
#define FOC_SENSORLESS_FIXED_FLUXOBSERVER_PLL_H
#include "other.h"
#include "User_Parameters.h"

struct FluxObserver_PLL_t {
    Q15_U_t Valpha_I;//输入Valpha(V)
    Q15_U_t Vbeta_I;//输入Vbeta(V)
    Q15_I_t Ialpha_I;//输入Ialpha(A)
    Q15_I_t Ibeta_I;//输入Ibeta(A)

    Q15_flux_t Flux_alpha_O;//输出转子磁链Flux_alpha(V·s)
    Q15_flux_t Flux_beta_O;//输出转子磁链Flux_beta(V·s)
    Q15_we_t Espeed_O;//输出电角速度Espeed(rad/s)
    Q15_te_t Etheta_O;//输出电角度Etheta(rad)
};
extern struct FluxObserver_PLL_t fluxObserver_pll_est;
void FluxObserver_init();//初始化FluxObserver,启动前必须执行一次
void FluxObserver_PLL_update(struct FluxObserver_PLL_t*fluxObserver_pll);

#endif //FOC_SENSORLESS_FIXED_FLUXOBSERVER_PLL_H
