//
// Created by lxcy on 2025/11/30.
//

#ifndef FOC_SENSORLESS_FIXED_PI_CONTROLLER_H
#define FOC_SENSORLESS_FIXED_PI_CONTROLLER_H
#include "fixed_point.h"
struct PI_Id_Controller_t {
    R2_t Set;//设定值
    R2_t Measure;//实测值
    R2_t error;//误差值(error=Set-Measure)
    R4_t Output;//输出值
    R10_t AddUp;//积分值
};
//Id的PI控制器，此代码为自动生成的代码，各项参数如下:
//比例增益P:10,积分增益乘以时间ITs:0.013333333,输出最小最大值:-6.5~6.5
static inline void Id_PI_update(struct PI_Id_Controller_t* Id){
     Id->error=R2_sub(Id->Set,Id->Measure);
     R4_t output_unsat =R4_add(
             R4_2_4_mul(R4_fromFloat(10),Id->error) ,
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
         Id->AddUp.value+=Id->error.value>>8;
     }
     Id->Output=output;
}
extern struct PI_Id_Controller_t Id_PIstate;
struct PI_Iq_Controller_t {
    R2_t Set;//设定值
    R2_t Measure;//实测值
    R2_t error;//误差值(error=Set-Measure)
    R4_t Output;//输出值
    R10_t AddUp;//积分值
};
//Iq的PI控制器，此代码为自动生成的代码，各项参数如下:
//比例增益P:10,积分增益乘以时间ITs:0.013333333,输出最小最大值:-6.5~6.5
static inline void Iq_PI_update(struct PI_Iq_Controller_t* Iq){
     Iq->error=R2_sub(Iq->Set,Iq->Measure);
     R4_t output_unsat =R4_add(
             R4_2_4_mul(R4_fromFloat(10),Iq->error) ,
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
         Iq->AddUp.value+=Iq->error.value>>8;
     }
     Iq->Output=output;
}
extern struct PI_Iq_Controller_t Iq_PIstate;
struct PI_Speed_Controller_t {
    R15_t Set;//设定值
    R15_t Measure;//实测值
    R15_t error;//误差值(error=Set-Measure)
    R2_t Output;//输出值
    R22_t AddUp;//积分值
};
//Speed的PI控制器，此代码为自动生成的代码，各项参数如下:
//比例增益P:0.0005,积分增益乘以时间ITs:6E-06,输出最小最大值:-1~1
static inline void Speed_PI_update(struct PI_Speed_Controller_t* Speed){
     Speed->error=R15_sub(Speed->Set,Speed->Measure);
     R2_t output_unsat =R2_add(
             R0_15_2_mul(R0_fromFloat(0.0005),Speed->error) ,
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
         Speed->AddUp.value+=Speed->error.value>>7;
     }
     Speed->Output=output;
}
extern struct PI_Speed_Controller_t Speed_PIstate;
struct PI_FluxObserver_Speed_Controller_t {
    R0_t Set;//设定值
    R0_t Measure;//实测值
    R0_t error;//误差值(error=Set-Measure)
    R15_t Output;//输出值
    R8_t AddUp;//积分值
};
//FluxObserver_Speed的PI控制器，此代码为自动生成的代码，各项参数如下:
//比例增益P:200000,积分增益乘以时间ITs:100,输出最小最大值:-5000~5000
static inline void FluxObserver_Speed_PI_update(struct PI_FluxObserver_Speed_Controller_t* FluxObserver_Speed){
    //FluxObserver_Speed->error=R0_sub(FluxObserver_Speed->Set,FluxObserver_Speed->Measure);
    R15_t output_unsat =R15_add(
            R18_0_15_mul(R18_fromFloat(200000),FluxObserver_Speed->error) ,
            R10_8_15_mul(R10_fromFloat(100),FluxObserver_Speed->AddUp)
            );
    R15_t output;
    bool saturated = false;
    if (R15_greater(output_unsat,R15_fromFloat(5000))) {
        output = R15_fromFloat(5000);
        saturated = true;
    } else if (R15_less(output_unsat,R15_fromFloat(-5000))) {
        output = R15_fromFloat(-5000);
        saturated = true;
    } else {
        output = output_unsat;
    }
    if (!saturated) {
        FluxObserver_Speed->AddUp.value+=FluxObserver_Speed->error.value>>8;
    }
    FluxObserver_Speed->Output=output;
}
extern struct PI_FluxObserver_Speed_Controller_t FluxObserver_Speed_PIstate;



#endif //FOC_SENSORLESS_FIXED_PI_CONTROLLER_H