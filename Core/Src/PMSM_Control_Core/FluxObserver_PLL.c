//
// Created by lxcy on 2026/7/14.
//
/* 磁链观测器与PLL，参考自论文
 * 《Performance Improvement of Nonlinear Flux Observer for Sensorless Control of PMSM》
 *                                  Jiali Liu and Yongchang Zhang , Senior Member, IEEE
 */
#include "PMSM_Control_Core/FluxObserver_PLL.h"
#include "PMSM_Control_Core/PI_Controller.h"
#include "PMSM_Control_Core/User_Parameters.h"

static Q15_flux_t flux_s[2] = {{0}}; //定子磁通预测值(V·s)
static Q15_flux_t flux_r[2] = {{0}, {0}}; //转子磁通预测值(V·s)
static Q15_flux_t flux_e = {0}; //转子磁通幅值(定值)
/*
 * 增益,仿真结果说明:太小时事实上就会使得磁链观测器事实上是无效的(甚至和lambda=0没有区别)
 * 可以查看
 * USB_data[USB_DataRecordIndex++]=fluxObserver_pll_est.Flux_alpha_O;
 * 和USB_data[USB_DataRecordIndex++]=fluxObserver_pll_est.Flux_beta_O;
 * 以alpha和beta的幅值对齐为妙(即alpha和beta的最大值和最小值都是相等的,而不是有一个偏置)
 * 但是不要过度放大，这里要注意Q15的最大值，防止溢出
 */
static const Q15_lambda_t lambdaDiv2 =(Q15_lambda_t){31000}; ////Q15_FromValue(1e7, lambda);
static Q15_te_t Etheta = Q15_FromValue(0.f, te); //电角度预测值(rad)
static Q15_we_t Espeed = Q15_FromValue(0.f, we); //电角速度预测值(rad/s)
static const float k = 1.f; //切向增益

struct FluxObserver_PLL_t fluxObserver_pll_est;

void FluxObserver_init() {
    flux_e = flux;
    flux_s[0] = flux;
}

static void FluxObserver_update(struct FluxObserver_PLL_t *fluxObserver_pll) {
    const Q15_U_t ualpha = fluxObserver_pll->Valpha_I;
    const Q15_U_t ubeta = fluxObserver_pll->Vbeta_I;
    const Q15_I_t ialpha = fluxObserver_pll->Ialpha_I;
    const Q15_I_t ibeta = fluxObserver_pll->Ibeta_I;
    flux_r[0] = Q15_C2C(Q15_sub(Q15_C2P(flux_s[0]),Q15_mul(Q15_C2P(Ls),Q15_C2P(ialpha))), flux);
    flux_r[1] = Q15_C2C(Q15_sub(Q15_C2P(flux_s[1]),Q15_mul(Q15_C2P(Ls),Q15_C2P(ibeta))), flux);

    //给定负转速时，必须要将k取相反数，否则导致磁链观测器无法收敛
    const float k_sign = Speed_PIstate.Set.child_value > 0 ? k * 1.f : k * -1.f; //给定负转速时，必须要将k取相反数，否则导致磁链观测器无法收敛
    flux_s[0] = Q15_C2C(
        Q15_add(
            Q15_mul(Q15_gain(_P(T_s),PI2),
                Q15_aa(_P(ualpha),Q15_mul(_NP(Rs),_P(ialpha)),
                    Q15_mm(_P(lambdaDiv2),
                        Q15_add(_P(flux_r[0]),Q15_gain(_P(flux_r[1]),-k_sign)),
                        Q15_mamam(
                            _P(flux_e),_P(flux_e),_NP(flux_r[0]),_P(flux_r[0]),_NP(flux_r[1]),_P(flux_r[1])
                        )
                    )
                )
            ),
            _P(flux_s[0])
        )
        , flux);
    flux_s[1] = Q15_C2C(
        Q15_add(
            Q15_mul(Q15_gain(_P(T_s),PI2),
                Q15_aa(_P(ubeta),Q15_mul(_NP(Rs),_P(ibeta)),
                    Q15_mm(_P(lambdaDiv2),
                        Q15_add(_P(flux_r[1]),Q15_gain(_P(flux_r[0]),k_sign)),
                        Q15_mamam(
                            _P(flux_e),_P(flux_e),_NP(flux_r[0]),_P(flux_r[0]),_NP(flux_r[1]),_P(flux_r[1])
                        )
                    )
                )
            ),
            _P(flux_s[1])
        )
        , flux);
    fluxObserver_pll->Flux_alpha_O = flux_r[0];
    fluxObserver_pll->Flux_beta_O = flux_r[1];
}

static void PLL_update(struct FluxObserver_PLL_t *fluxObserver_pll) {
    Q15_t c_theta, s_theta;
    Q15_sin_cos(Etheta, &s_theta, &c_theta);
    const Q15_flux_t err = Q15_C2C(
        Q15_mam(_P(fluxObserver_pll->Flux_beta_O),c_theta,_NP(fluxObserver_pll->Flux_alpha_O),s_theta), flux);
    FluxObserver_PLLSpeed_PIstate.error = _P(err);
    FluxObserver_PLLSpeed_PI_update(&FluxObserver_PLLSpeed_PIstate);
    Espeed = FluxObserver_PLLSpeed_PIstate.Output;
    Etheta=Q15_C2C(Q15_add(_P(Etheta),Q15_gain(Q15_mul(_P(Espeed),_P(T_s)),2.f)),te);//这里需要手动乘以2以转换成Etheta对应的基准值

    static const uint8_t useLPF_Speed = 1; //使用低通滤波得到更平滑的转速输出(500hz截止频率)
    if (useLPF_Speed) {
        fluxObserver_pll->Espeed_O = lowPass_filter_flux_Espeed(Espeed);
    } else {
        fluxObserver_pll->Espeed_O = Espeed;
    }
    fluxObserver_pll->Etheta_O = Etheta;
}

void FluxObserver_PLL_update(struct FluxObserver_PLL_t *fluxObserver_pll) {
    FluxObserver_update(fluxObserver_pll);
    PLL_update(fluxObserver_pll);
}
