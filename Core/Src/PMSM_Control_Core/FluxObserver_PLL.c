//
// Created by lxcy on 2025/12/19.
//
#include "PMSM_Control_Core/FluxObserver_PLL.h"

#include "PMSM_Control_Core/PI_Controller.h"

static constexpr float T_s=1.0f/15000;//采样时间(SMO执行周期)(s)
static constexpr float Rs=5.5f/2;//电阻(Ohm)
static constexpr float Ls=2.e-3f/2;;//电感(H)
static R0_t flux_s[2]={R0_fromFloat(0.0050628f),R0_fromFloat(0.f)};//定子磁通预测值(V·s)
static R0_t flux_r[2]={R0_fromFloat(0.f),R0_fromFloat(0.f)};//转子磁通预测值(V·s)
static constexpr float flux_e=0.0050628f;//转子磁通幅值(定值)
static constexpr float lambda=600.f;//增益
static R4_t Etheta=R4_fromFloat(0.f);//电角度预测值(rad)
static R15_t Espeed=R15_fromFloat(0.f);//电角速度预测值(rad/s)
static constexpr float k=1.f;//切向增益(以下代码设计时假定k∈(-4,4),调参时不应超出该范围)

struct FluxObserver_PLL_t fluxObserver_pll_est;

static void FluxObserver_update(struct FluxObserver_PLL_t*fluxObserver_pll_est) {
    const R4_t ualpha=fluxObserver_pll_est->Valpha_I;
    const R4_t ubeta=fluxObserver_pll_est->Vbeta_I;
    const R2_t ialpha=fluxObserver_pll_est->Ialpha_I;
    const R2_t ibeta=fluxObserver_pll_est->Ibeta_I;
    flux_r[0]=R0_sub(flux_s[0],R2_f0_0_mul(ialpha,Ls));
    flux_r[1]=R0_sub(flux_s[1],R2_f0_0_mul(ibeta,Ls));

    flux_s[0]=R0_add(
        R4_f0_0_mul(
            R4_add_3(
            ualpha,
            R2_f2_4_mul(ialpha,-Rs),
            R0_f10_4_mul(
                R0_0_0_mul(
                    R0_sub(flux_r[0],R0_f2_0_mul(flux_r[1],k))
                    ,R0_sub_3(R0_fromFloat(flux_e*flux_e),
                        R0_0_0_mul(flux_r[0],flux_r[0]),
                        R0_0_0_mul(flux_r[1],flux_r[1])
                        )
                    )
                ,lambda/2)
            )
            ,T_s)
        ,flux_s[0]);
    flux_s[1]=R0_add(
        R4_f0_0_mul(
            R4_add_3(
            ubeta,
            R2_f2_4_mul(ibeta,-Rs),
            R0_f10_4_mul(
                R0_0_0_mul(
                    R0_add(flux_r[1],R0_f2_0_mul(flux_r[0],k))
                    ,R0_sub_3(R0_fromFloat(flux_e*flux_e),
                        R0_0_0_mul(flux_r[0],flux_r[0]),
                        R0_0_0_mul(flux_r[1],flux_r[1])
                        )
                    )
                ,lambda/2)
            )
            ,T_s)
        ,flux_s[1]);
    fluxObserver_pll_est->Flux_alpha_O=flux_r[0];
    fluxObserver_pll_est->Flux_beta_O=flux_r[1];
}

static void PLL_update(struct FluxObserver_PLL_t*fluxObserver_pll_est) {
    R0_t c_theta,s_theta;
    R4_sin_cos(Etheta,&s_theta,&c_theta);
    const R0_t err=R0_sub(R0_0_0_mul(fluxObserver_pll_est->Flux_beta_O,c_theta),R0_0_0_mul(fluxObserver_pll_est->Flux_alpha_O,s_theta));
    FluxObserver_Speed_PIstate.error=err;
    FluxObserver_Speed_PI_update(&FluxObserver_Speed_PIstate);
    Espeed=FluxObserver_Speed_PIstate.Output;
    Etheta=R4_add(Etheta,R15_f0_4_mul(Espeed,T_s));
    while (Etheta.value >= R4_fromFloat(PI2).value) Etheta.value -= R4_fromFloat(PI2).value;
    while (Etheta.value < R4_fromFloat(0).value) Etheta.value +=R4_fromFloat(PI2).value;
    fluxObserver_pll_est->Espeed_O=Espeed;
    fluxObserver_pll_est->Etheta_O=Etheta;
}

void FluxObserver_PLL_update(struct FluxObserver_PLL_t*fluxObserver_pll_est) {
    FluxObserver_update(fluxObserver_pll_est);
    PLL_update(fluxObserver_pll_est);
}