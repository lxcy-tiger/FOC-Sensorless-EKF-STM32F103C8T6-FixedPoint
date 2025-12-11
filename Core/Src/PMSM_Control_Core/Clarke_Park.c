//
// Created by lxcy on 2025/11/30.
//

#include "PMSM_Control_Core/Clarke_Park.h"

//Clarke和Park变换的所有变量，使用一个结构体全部包含(I表示输入,O表示输出)
struct Clarke_Park_t clarke_park_transform={0};

//Clarke变换
void Clarke_transform(struct Clarke_t* clarke) {
    clarke->Ialpha_O = clarke->Ia_I;
    clarke->Ibeta_O =R2_f0_2_mul(R2_add(clarke->Ia_I,R2_f2_2_mul(clarke->Ib_I,2)),Div_Sqrt_3);
}

//反Clarke变换
void IClarke_transform(struct IClarke_t* iclarke) {
    iclarke->Va_O=iclarke->Valpha_I;
    iclarke->Vb_O= R4_f0_4_mul(R4_sub(R4_f1_4_mul(iclarke->Vbeta_I,Sqrt_3),iclarke->Valpha_I),0.5f);
    iclarke->Vc_O=R4_neg(R4_f0_4_mul(R4_add(iclarke->Valpha_I,R4_f1_4_mul(iclarke->Vbeta_I,Sqrt_3)),0.5f));
}

//Park变换
void Park_transform(struct Park_t* park) {
    R0_t cosTheta,sinTheta;
    R4_sin_cos(park->Theta_I,&sinTheta,&cosTheta);
    park->Id_O=R2_add(R2_0_2_mul(park->Ialpha_I,cosTheta),R2_0_2_mul(park->Ibeta_I,sinTheta));
    park->Iq_O=R2_sub(R2_0_2_mul(park->Ibeta_I,cosTheta),R2_0_2_mul(park->Ialpha_I,sinTheta));
}

//反Park变换
void IPark_transform(struct IPark_t* ipark) {
    R0_t cosTheta,sinTheta;
    R4_sin_cos(ipark->Theta_I,&sinTheta,&cosTheta);
    ipark->Valpha_O=R4_sub(R4_0_4_mul(ipark->Vd_I,cosTheta),R4_0_4_mul(ipark->Vq_I,sinTheta));
    ipark->Vbeta_O=R4_add(R4_0_4_mul(ipark->Vd_I,sinTheta),R4_0_4_mul(ipark->Vq_I,cosTheta));
}

/*
精度测试:
R0_t Gs=R0_fromFloat(0.2f);//三相相电导
int main(void) {
    ClarkePark.ipark.Vd_I=R4_fromFloat(0);
    ClarkePark.ipark.Vq_I=R4_fromFloat(6);
    for (int i=0;i<360;++i) {
        ClarkePark.ipark.Theta_I=R4_fromFloat(i*PI2/360);
        IPark_transform(&ClarkePark.ipark);
        ClarkePark.iclarke.Valpha_I=ClarkePark.ipark.Valpha_O;
        ClarkePark.iclarke.Vbeta_I=ClarkePark.ipark.Vbeta_O;
        IClarke_transform(&ClarkePark.iclarke);
        ClarkePark.clarke.Ia_I=R4_0_2_mul(ClarkePark.iclarke.Va_O,Gs);
        ClarkePark.clarke.Ib_I=R4_0_2_mul(ClarkePark.iclarke.Vb_O,Gs);
        ClarkePark.clarke.Ic_I=R4_0_2_mul(ClarkePark.iclarke.Vc_O,Gs);
        Clarke_transform(&ClarkePark.clarke);
        ClarkePark.park.Ialpha_I=ClarkePark.clarke.Ialpha_O;
        ClarkePark.park.Ibeta_I=ClarkePark.clarke.Ibeta_O;
        ClarkePark.park.Theta_I=R4_fromFloat(i*PI2/360);
        Park_transform(&ClarkePark.park);
        printf("theta:%f,Id:%f,Iq:%f\n",cvtRToFloat(R4_fromFloat(i*PI2/360)),
            cvtRToFloat(ClarkePark.park.Id_O),cvtRToFloat(ClarkePark.park.Iq_O));
    }
    return 0;
}
输出结果在1.198~1.2之间，说明误差在0.002/1.2=0.17%以内
 */