//
// Created by lxcy on 2025/11/30.
//
#include "PMSM_Control_Core/EKF.h"

constexpr float Rs=5.5f/2; //电阻(Ohm)
constexpr float Ls=2.e-3f/2; //电感(H)
constexpr float flux=0.00386335f;//磁通(V·s)
constexpr float T_s=1.f/15000;//采样时间(EKF执行周期)(s)
constexpr float TsDivLs=T_s/Ls;//数值上等于T_s/Ls，用于加速计算过程
constexpr float RsTsDivLs=Rs*TsDivLs;//数值上等于Rs*T_s/Ls，用于加速计算过程
constexpr float fluxTsDivLs=flux*TsDivLs;//数值上等于flux*T_s/Ls，用于加速计算过程
constexpr float Q_0_0=0.03f;constexpr float Q_1_1=0.03f;
    constexpr float Q_2_2=100.f;constexpr float Q_3_3=0.00003f; //过程噪声方差矩阵
constexpr float R_0_0=0.05f;constexpr float R_1_1=0.05f; //测量噪声方差矩阵
static R2_t x_k_k_prev_0,x_k_k_prev_1;R15_t x_k_k_prev_2;R4_t x_k_k_prev_3; //先验估计
static R2_t x_k_k_0,x_k_k_1;R15_t x_k_k_2;R4_t x_k_k_3; //后验估计
static R0_t P_k_k_prev_0_0,P_k_k_prev_0_1;static R8_t P_k_k_prev_0_2;static R0_t P_k_k_prev_0_3;
        static R0_t P_k_k_prev_1_1;static R8_t P_k_k_prev_1_2;static R0_t P_k_k_prev_1_3;
        static R15_t P_k_k_prev_2_2;static R8_t P_k_k_prev_2_3;
        static R2_t P_k_k_prev_3_3; //先验方差矩阵
static R0_t P_k_k_0_0=R0_fromFloat(0.03),P_k_k_0_1;static R8_t P_k_k_0_2;static R0_t P_k_k_0_3;
        static R0_t P_k_k_1_1=R0_fromFloat(0.035);static R8_t P_k_k_1_2;static R0_t P_k_k_1_3;
        static R15_t P_k_k_2_2=R15_fromFloat(1);static R8_t P_k_k_2_3;
        static R2_t P_k_k_3_3=R2_fromFloat(0.5);  //后验方差矩阵
static R0_t K_k_0_0,K_k_0_1,K_k_1_0,K_k_1_1,K_k_3_0,K_k_3_1;
        static R8_t K_k_2_0,K_k_2_1; //卡尔曼增益
struct EKF_t ekf_est;

void EKF_update(struct EKF_t*ekf) {
    const R2_t i_alpha_k_prev = x_k_k_0;
    const R2_t i_beta_k_prev = x_k_k_1;
    const R15_t w_e_k_prev =x_k_k_2;
    const R4_t theta_e_k_prev = x_k_k_3;
    R0_t c_theta_e_k_prev,s_theta_e_k_prev;//不能为const
    R4_sin_cos(theta_e_k_prev,&s_theta_e_k_prev,&c_theta_e_k_prev);

    x_k_k_prev_0=R2_add_3(
        R2_f0_2_mul(i_alpha_k_prev,1-RsTsDivLs),
        R15_f0_2_mul(R15_0_15_mul(w_e_k_prev,s_theta_e_k_prev),fluxTsDivLs),
        R4_f0_2_mul(ekf->Valpha_I,TsDivLs)
        );
    x_k_k_prev_1=R2_add_3(
        R2_f0_2_mul(i_beta_k_prev,1-RsTsDivLs),
        R15_f0_2_mul(R15_0_15_mul(w_e_k_prev,c_theta_e_k_prev),-fluxTsDivLs),
        R4_f0_2_mul(ekf->Vbeta_I,TsDivLs)
        );
    x_k_k_prev_2 = w_e_k_prev;
    x_k_k_prev_3 = R4_add(
        theta_e_k_prev,
        R15_f4_4_mul(w_e_k_prev,T_s)
        );

    constexpr float A_k_0_0=1 - Rs*TsDivLs;
    constexpr float A_k_1_1=1 - Rs*TsDivLs;
    constexpr float A_k_3_2=T_s;
    const R0_t A_k_0_2=R0_f0_0_mul(s_theta_e_k_prev,fluxTsDivLs);
    const R0_t A_k_1_2= R0_f0_0_mul(c_theta_e_k_prev,-fluxTsDivLs);
    const R0_t A_k_0_3=R15_0_0_mul(R15_neg(w_e_k_prev),A_k_1_2);
    const R0_t A_k_1_3 =R15_0_0_mul(w_e_k_prev,A_k_0_2);

    //计算P_k_k_prev
    P_k_k_prev_0_0=R0_add_6(
            R0_f0_0_mul(P_k_k_0_0,A_k_0_0*A_k_0_0),
            R0_f1_0_mul(R0_add(R8_0_0_mul(P_k_k_0_2,A_k_0_2),R0_0_0_mul(A_k_0_3,P_k_k_0_3)),2*A_k_0_0),
            R15_0_0_mul(R15_0_15_mul(P_k_k_2_2,A_k_0_2),A_k_0_2),
            R0_f2_0_mul(R2_0_0_mul(R8_0_2_mul(P_k_k_2_3,A_k_0_3),A_k_0_2),2),
            R0_0_0_mul(R2_0_0_mul(P_k_k_3_3,A_k_0_3),A_k_0_3),
            R0_fromFloat(Q_0_0)
        );
    P_k_k_prev_0_1=R0_add_5(
        R0_f0_0_mul(P_k_k_0_1,A_k_0_0*A_k_1_1),
        R0_f0_0_mul(R0_add(R8_0_0_mul(P_k_k_1_2,A_k_0_2),R0_0_0_mul(A_k_0_3,P_k_k_1_3)),A_k_1_1),
        R0_f0_0_mul(R0_add(R8_0_0_mul(P_k_k_0_2,A_k_1_2),R0_0_0_mul(A_k_1_3,P_k_k_0_3)),A_k_0_0),
        R2_0_0_mul(R2_add(R15_0_2_mul(P_k_k_2_2,A_k_0_2),R8_0_2_mul(P_k_k_2_3,A_k_0_3)),A_k_1_2),
        R0_0_0_mul(R0_add(R8_0_0_mul(P_k_k_2_3,A_k_0_2),R2_0_0_mul(P_k_k_3_3,A_k_0_3)),A_k_1_3)
        );
    P_k_k_prev_1_1=R0_add_6(
        R0_f0_0_mul(P_k_k_1_1,A_k_1_1*A_k_1_1),
        R0_f1_0_mul(R0_add(R8_0_0_mul(P_k_k_1_2,A_k_1_2),R0_0_0_mul(P_k_k_1_3,A_k_1_3)),2*A_k_1_1),
        R2_0_0_mul(R15_0_2_mul(P_k_k_2_2,A_k_1_2),A_k_1_2),
        R0_f2_0_mul(R2_0_0_mul(R8_0_2_mul(P_k_k_2_3,A_k_1_3),A_k_1_2),2),
        R0_0_0_mul(R2_0_0_mul(P_k_k_3_3,A_k_1_3),A_k_1_3),
        R0_fromFloat(Q_1_1)
        );
    P_k_k_prev_2_2=R15_add(P_k_k_2_2,R15_fromFloat(Q_2_2));

    P_k_k_prev_3_3=R2_add_4(
        R2_f0_2_mul(R15_f0_2_mul(P_k_k_2_2,A_k_3_2),A_k_3_2),
        R8_f0_2_mul(P_k_k_2_3,2*A_k_3_2),
        P_k_k_3_3,
        R2_fromFloat(Q_3_3)
        );
    P_k_k_prev_2_3=R8_add(R15_f0_8_mul(P_k_k_2_2,A_k_3_2),P_k_k_2_3);

    const R8_t APpBP_0_0=R8_add_3(
        R8_f0_8_mul(P_k_k_0_2,A_k_0_0),
        R15_0_8_mul(P_k_k_2_2,A_k_0_2),
        R8_0_8_mul(P_k_k_2_3,A_k_0_3)
        );
    const R0_t APpBP_0_1=R0_add_3(
        R0_f0_0_mul(P_k_k_0_3,A_k_0_0),
        R8_0_0_mul(P_k_k_2_3,A_k_0_2),
        R2_0_0_mul(P_k_k_3_3,A_k_0_3)
        );
    const R8_t APpBP_1_0=R8_add_3(
        R8_f0_8_mul(P_k_k_1_2,A_k_1_1),
        R15_0_8_mul(P_k_k_2_2,A_k_1_2),
        R8_0_8_mul(P_k_k_2_3,A_k_1_3)
        );
    const R0_t APpBP_1_1=R0_add_3(
        R0_f0_0_mul(P_k_k_1_3,A_k_1_1),
        R8_0_0_mul(P_k_k_2_3,A_k_1_2),
        R2_0_0_mul(P_k_k_3_3,A_k_1_3)
        );
    P_k_k_prev_0_2=APpBP_0_0;
    P_k_k_prev_1_2=APpBP_1_0;
    P_k_k_prev_0_3=R0_add(R8_f0_0_mul(APpBP_0_0,A_k_3_2),APpBP_0_1);
    P_k_k_prev_1_3=R0_add(R8_f0_0_mul(APpBP_1_0,A_k_3_2),APpBP_1_1);

    //计算Kk
    const R0_t PR00=R0_add(P_k_k_prev_0_0,R0_fromFloat(R_0_0));
    const R0_t PR11=R0_add(P_k_k_prev_1_1,R0_fromFloat(R_1_1));
    const R0_t PR01=R0_0_0_mul(P_k_k_prev_0_1,P_k_k_prev_0_1);
    const R8_t det=R0_inv(R0_sub(R0_0_0_mul(PR00,PR11),PR01));
    K_k_0_0=R8_0_0_mul(det,R0_sub(R0_0_0_mul(P_k_k_prev_0_0,PR11),PR01));
    K_k_0_1=R8_0_0_mul(det,R0_f0_0_mul(P_k_k_prev_0_1,R_0_0));
    K_k_1_0=R8_0_0_mul(det,R0_f0_0_mul(P_k_k_prev_0_1,R_1_1));
    K_k_1_1=R8_0_0_mul(det,R0_sub(R0_0_0_mul(P_k_k_prev_1_1,PR00),PR01));
    K_k_2_0=R8_0_8_mul(det,R0_sub(R8_0_0_mul(P_k_k_prev_0_2,PR11),R8_0_0_mul(P_k_k_prev_1_2,P_k_k_prev_0_1)));
    K_k_2_1=R8_0_8_mul(det,R0_sub(R8_0_0_mul(P_k_k_prev_1_2,PR00),R8_0_0_mul(P_k_k_prev_0_2,P_k_k_prev_0_1)));
    K_k_3_0=R8_0_0_mul(det,R0_sub(R0_0_0_mul(P_k_k_prev_0_3,PR11),R0_0_0_mul(P_k_k_prev_1_3,P_k_k_prev_0_1)));
    K_k_3_1=R8_0_0_mul(det,R0_sub(R0_0_0_mul(P_k_k_prev_1_3,PR00),R0_0_0_mul(P_k_k_prev_0_3,P_k_k_prev_0_1)));

    //ypCx=y-Cx~(k-1)=y-Cf(xk-1|k-1,uk,0)=Cxk|k-1
    R2_t ypCx[2];
    ypCx[0]=R2_sub(ekf->Ialpha_I,x_k_k_prev_0);
    ypCx[1]=R2_sub(ekf->Ibeta_I,x_k_k_prev_1);


    x_k_k_0 =R2_add_3(x_k_k_prev_0,R2_0_2_mul(ypCx[0],K_k_0_0),R2_0_2_mul(ypCx[1],K_k_0_1));
    x_k_k_1 =R2_add_3(x_k_k_prev_1,R2_0_2_mul(ypCx[0],K_k_1_0),R2_0_2_mul(ypCx[1],K_k_1_1));
    x_k_k_2 =R15_add_3(x_k_k_prev_2,R8_2_15_mul(K_k_2_0,ypCx[0]),R8_2_15_mul(K_k_2_1,ypCx[1]));
    x_k_k_3 =R4_add_3(x_k_k_prev_3,R2_0_4_mul(ypCx[0],K_k_3_0),R2_0_4_mul(ypCx[1],K_k_3_1));

    //更新协方差矩阵
    P_k_k_0_0=R0_sub_3(P_k_k_prev_0_0,R0_0_0_mul(K_k_0_0,P_k_k_prev_0_0),R0_0_0_mul(K_k_0_1,P_k_k_prev_0_1));
    P_k_k_0_1=R0_sub_3(P_k_k_prev_0_1,R0_0_0_mul(K_k_0_0,P_k_k_prev_0_1),R0_0_0_mul(K_k_0_1,P_k_k_prev_1_1));
    P_k_k_0_2=R8_sub_3(P_k_k_prev_0_2,R8_0_8_mul(P_k_k_prev_0_2,K_k_0_0),R8_0_8_mul(P_k_k_prev_1_2,K_k_0_1));
    P_k_k_0_3=R0_sub_3(P_k_k_prev_0_3,R0_0_0_mul(K_k_0_0,P_k_k_prev_0_3),R0_0_0_mul(K_k_0_1,P_k_k_prev_1_3));

    P_k_k_1_1=R0_sub_3(P_k_k_prev_1_1,R0_0_0_mul(K_k_1_0,P_k_k_prev_0_1),R0_0_0_mul(K_k_1_1,P_k_k_prev_1_1));
    P_k_k_1_2=R8_sub_3(P_k_k_prev_1_2,R8_0_8_mul(P_k_k_prev_0_2,K_k_1_0),R8_0_8_mul(P_k_k_prev_1_2,K_k_1_1));
    P_k_k_1_3=R0_sub_3(P_k_k_prev_1_3,R0_0_0_mul(K_k_1_0,P_k_k_prev_0_3),R0_0_0_mul(K_k_1_1,P_k_k_prev_1_3));

    P_k_k_2_2=R15_sub_3(P_k_k_prev_2_2,R8_8_15_mul(K_k_2_0,P_k_k_prev_0_2),R8_8_15_mul(K_k_2_1,P_k_k_prev_1_2));
    P_k_k_2_3=R8_sub_3(P_k_k_prev_2_3,R8_0_8_mul(K_k_2_0,P_k_k_prev_0_3),R8_0_8_mul(K_k_2_1,P_k_k_prev_1_3));

    P_k_k_3_3=R2_sub_3(P_k_k_prev_3_3,R0_0_2_mul(K_k_3_0,P_k_k_prev_0_3),R0_0_2_mul(K_k_3_1,P_k_k_prev_1_3));

    while (R4_greater(x_k_k_3,PI2_R4)) x_k_k_3=R4_sub(x_k_k_3,PI2_R4);
    while (R4_float_less(x_k_k_3,0)) x_k_k_3=R4_add(x_k_k_3,PI2_R4);

    ekf->Ialpha_O=x_k_k_0;
    ekf->Ibeta_O=x_k_k_1;
    ekf->Espeed_O=x_k_k_2;
    ekf->Etheta_O=x_k_k_3;
}

/*
 性能分析:
 全局O2,调试模式计算:
__attribute__((optimize("O0")))
int main(void)
{
........................
for (int i=0;i<100000;++i) {
    ekf_est.Ialpha_I.value=i;
    ekf_est.Ibeta_I.value=i;
    ekf_est.Valpha_I.value=i;
    ekf_est.Ibeta_I.value=i;
    EKF_update(&ekf_est);
}
uint32_t time=DWT->CYCCNT;

得到time=186302371,单片机为72Mhz,于是执行100000次为186302371/72M=2.58753293s
执行1次为2.59e-5s，即25.9us
占20khz的FOC的50%CPU时间

单次计时:
__attribute__((optimize("O0")))
int main(void)
{
........................
ekf_est.Ialpha_I.value=3;
ekf_est.Ibeta_I.value=3;
ekf_est.Valpha_I.value=3;
ekf_est.Ibeta_I.value=3;
EKF_update(&ekf_est);
uint32_t time=DWT->CYCCNT;
得到time=1757,单片机为72Mhz,于是执行1次为1757/72M=24.4us
与100000次结果接近
 */