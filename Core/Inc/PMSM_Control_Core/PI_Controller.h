//
// Created by lxcy on 2025/11/30.
//

#ifndef FOC_SENSORLESS_FIXED_PI_CONTROLLER_H
#define FOC_SENSORLESS_FIXED_PI_CONTROLLER_H
#include "fixed_point.h"
struct PI_Id_Controller_t {
    R2_t Set;//设定值
    R2_t Measure;//实测值
    //error=Set-Measure
    R4_t Output;//输出值
    R10_t AddUp;//积分值
};
//Id的PI控制器，此代码为自动生成的代码，各项参数如下:
//比例增益P:10,积分增益乘以时间ITs:0.013333333,输出最小最大值:-6.5~6.5
static inline void Id_PI_update(struct PI_Id_Controller_t* Id){
     R2_t error=R2_sub(Id->Set,Id->Measure);
     R4_t output_unsat =R4_add(
             R4_2_4_mul(R4_fromFloat(10),error) ,
             R0_10_4_mul(R0_fromFloat(0.013333333),Id->AddUp)
             );
     R4_t output;
     bool saturated = false;
     if (R4_greater(output_unsat,R4_fromFloat(6.5))) {
         output = R4_fromFloat(6.5);
         saturated = true;
     } else if (R4_less(output_unsat,R4_fromFloat(-6.5))) {
         output = R4_fromFloat(-6.5);
         saturated = true;
     } else {
     output = output_unsat;
     }
     if (!saturated) {
         Id->AddUp.value+=error.value>>8;
     }
     Id->Output=output;
}
extern struct PI_Id_Controller_t Id_PIstate;
struct PI_Iq_Controller_t {
    R2_t Set;//设定值
    R2_t Measure;//实测值
    //error=Set-Measure
    R4_t Output;//输出值
    R10_t AddUp;//积分值
};
//Iq的PI控制器，此代码为自动生成的代码，各项参数如下:
//比例增益P:10,积分增益乘以时间ITs:0.013333333,输出最小最大值:-6.5~6.5
static inline void Iq_PI_update(struct PI_Iq_Controller_t* Iq){
     R2_t error=R2_sub(Iq->Set,Iq->Measure);
     R4_t output_unsat =R4_add(
             R4_2_4_mul(R4_fromFloat(10),error) ,
             R0_10_4_mul(R0_fromFloat(0.013333333),Iq->AddUp)
             );
     R4_t output;
     bool saturated = false;
     if (R4_greater(output_unsat,R4_fromFloat(6.5))) {
         output = R4_fromFloat(6.5);
         saturated = true;
     } else if (R4_less(output_unsat,R4_fromFloat(-6.5))) {
         output = R4_fromFloat(-6.5);
         saturated = true;
     } else {
     output = output_unsat;
     }
     if (!saturated) {
         Iq->AddUp.value+=error.value>>8;
     }
     Iq->Output=output;
}
extern struct PI_Iq_Controller_t Iq_PIstate;
struct PI_Speed_Controller_t {
    R15_t Set;//设定值
    R15_t Measure;//实测值
    //error=Set-Measure
    R2_t Output;//输出值
    R22_t AddUp;//积分值
};
//Speed的PI控制器，此代码为自动生成的代码，各项参数如下:
//比例增益P:0.0005,积分增益乘以时间ITs:6E-06,输出最小最大值:-1~1
static inline void Speed_PI_update(struct PI_Speed_Controller_t* Speed){
     R15_t error=R15_sub(Speed->Set,Speed->Measure);
     R2_t output_unsat =R2_add(
             R0_15_2_mul(R0_fromFloat(0.0005),error) ,
             R0_22_2_mul(R0_fromFloat(6E-06),Speed->AddUp)
             );
     R2_t output;
     bool saturated = false;
     if (R2_greater(output_unsat,R2_fromFloat(1))) {
         output = R2_fromFloat(1);
         saturated = true;
     } else if (R2_less(output_unsat,R2_fromFloat(-1))) {
         output = R2_fromFloat(-1);
         saturated = true;
     } else {
     output = output_unsat;
     }
     if (!saturated) {
         Speed->AddUp.value+=error.value>>7;
     }
     Speed->Output=output;
}

extern struct PI_Speed_Controller_t Speed_PIstate;



#endif //FOC_SENSORLESS_FIXED_PI_CONTROLLER_H