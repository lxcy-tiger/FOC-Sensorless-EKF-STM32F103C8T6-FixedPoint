//
// Created by lxcy on 2026/7/27.
//
#include "PMSM_Control_Core/Clarke_Park.h"

#include "other.h"
//Clarke和Park变换的所有变量，使用一个结构体全部包含(I表示输入,O表示输出)
struct Clarke_Park_t clarke_park_transform={0};

//Clarke变换
void Clarke_transform(struct Clarke_t* clarke) {
    clarke->Ialpha_O = clarke->Ia_I;
    clarke->Ibeta_O =Q15_C2C(Q15_add(Q15_gain(_P(clarke->Ia_I),Div_Sqrt_3),Q15_gain(_P(clarke->Ib_I),2*Div_Sqrt_3)),I);
}

//反Clarke变换
void IClarke_transform(struct IClarke_t* iclarke) {
    iclarke->Va_O=iclarke->Valpha_I;
    iclarke->Vb_O=Q15_C2C(Q15_sub(Q15_gain(_P(iclarke->Vbeta_I),Sqrt_3*0.5),Q15_gain(_P(iclarke->Valpha_I),0.5)),U);
    iclarke->Vc_O=Q15_C2C(Q15_add(Q15_gain(_NP(iclarke->Valpha_I),0.5),Q15_gain(_NP(iclarke->Vbeta_I),Sqrt_3*0.5)),U);
}

//Park变换
void Park_transform(struct Park_t* park) {
    Q15_t cosTheta,sinTheta;
    Q15_sin_cos(park->Theta_I,&sinTheta,&cosTheta);
    park->Id_O=Q15_C2C(Q15_mam(cosTheta,_P(park->Ialpha_I),sinTheta,_P(park->Ibeta_I)),I);
    park->Iq_O=Q15_C2C(Q15_mam(sinTheta,_NP(park->Ialpha_I),cosTheta,_P(park->Ibeta_I)),I);
}

//反Park变换
void IPark_transform(struct IPark_t* ipark) {
    Q15_t cosTheta,sinTheta;
    Q15_sin_cos(ipark->Theta_I,&sinTheta,&cosTheta);
    ipark->Valpha_O=Q15_C2C(Q15_mam(cosTheta,_P(ipark->Vd_I),sinTheta,_NP(ipark->Vq_I)),U);
    ipark->Vbeta_O=Q15_C2C(Q15_mam(sinTheta,_P(ipark->Vd_I),cosTheta,_P(ipark->Vq_I)),U);
}