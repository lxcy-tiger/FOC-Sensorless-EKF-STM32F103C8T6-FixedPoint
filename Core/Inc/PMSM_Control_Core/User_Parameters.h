//
// Created by lxcy on 2026/7/12.
//

#ifndef FOC_SENSORLESS_FIXED_USER_PARAMETERS_H
#define FOC_SENSORLESS_FIXED_USER_PARAMETERS_H
#include <math.h>

#include "fixed_point.h"
static const float Udc=12.f;//电机额定电压(V)
static const float I_max=1.0f;//电机最大电流(A)
static const float we_max=1800.f;//电机额定电角速度(rad/s)

Q15_baseValue(U,Udc/1.732f*1.4f);//电压基值(V)，通常取电机额定电压除以根号三(但是在SVPWM中计算有溢出，故选择为额定电压)
Q15_baseValue(I,I_max*1.2f);//电流基值(A)，通常取电机最大电流，这里留有一定裕量
Q15_baseValue(Z,U_q15base.base_value/I_q15base.base_value);//阻抗基值(Ω)，通常取电压基值除以电流基值
Q15_baseValue(we,we_max*1.1f);//电角速度基值(rad/s)，通常取电机最大电角速度，这里留有一点裕量
Q15_baseValue(L,Z_q15base.base_value/we_q15base.base_value);//电感基值(H)，通常取电阻基值除以电角速度基值
Q15_baseValue(C,1/Z_q15base.base_value/we_q15base.base_value);//电容基值(F),通常取电阻基值与电角速度基值的倒数
Q15_baseValue(flux,U_q15base.base_value/we_q15base.base_value);//磁链基值(V·s/rad),通常取电压基值除以电角速度基值
Q15_baseValue(T,2*M_PI/we_q15base.base_value);//时间基值(s)，由量纲分析，取2PI除以角速度基值
Q15_baseValue(te,M_PI);//电角度基值(rad/2),范围是-PI~PI,将映射到-32768~32767,注意这里不用2*M_PI,以保证精度以及自动溢出钳位功能,但是缺点是需要手动转换单位
Q15_baseValue(lambda,U_q15base.base_value/flux_q15base.base_value/flux_q15base.base_value/flux_q15base.base_value)//lambda基值(由量纲分析得到V/(rad/s)^3，因此应该取Ubase/fluxbase^3),专用于改进的非线性磁链观测器。


static const Q15_Z_t Rs=Q15_FromValue(5.5f/2,Z); //电阻(Ohm)
static const Q15_L_t Ls= Q15_FromValue(2.e-3f/2,L); //电感(H)
static const Q15_flux_t flux =Q15_FromValue(0.00386335f,flux); //磁通(V·s/rad)
static const float J=4.5e-6f;//机械转动惯量(kg·m^2),暂时不用,保持float格式
static const Q15_T_t T_s=Q15_FromValue(1.f/15000,T);//观测器执行周期、控制周期(s)
static const Q15_T_t T_s_rad=Q15_FromValue(2*M_PI/15000,T);//观测器执行周期、控制周期(s)
static const uint8_t POLE_PAIRS=7; //极对数

static const float Current_wc=12566;//电流环截止频率(rad/s)
static const float Current_MaxOutput=Udc/M_SQRT3*0.90;//电流环最大输出值(V),双电阻采样时,达不到电压极限圆,需要留有一定裕量以留出电流采样窗口
static const float Speed_wc=70;//转速环截止频率(rad/s)
static const float Speed_MaxOutput=I_max;//转速环最大输出值(A)
static const float FluxPLL_wc=80;//磁链观测器锁相环截止频率(rad/s)
static const float FluxPLL_MaxOutput=we_max;//磁链观测器锁相环输出最大值(rad/s)


#endif //FOC_SENSORLESS_FIXED_USER_PARAMETERS_H
