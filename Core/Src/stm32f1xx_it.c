/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f1xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_it.h"



/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "USB_JustFloat.h"
#include "PMSM_Control_Core/Clarke_Park.h"
#include "PMSM_Control_Core/FluxObserver_PLL.h"
#include "PMSM_Control_Core/Hardware.h"
#include "PMSM_Control_Core/PI_Controller.h"
#include "PMSM_Control_Core/SVPWM.h"
#include "PMSM_Control_Core/User_Parameters.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/*
 *  IF强拖切闭环步骤
 *  数字0:IF强拖步骤,速度不停增加(IdIq电流环开启,转速环开启,观测器开启,角度:由IF给定)
 *        数字0->数字1:单周期坐标变换，瞬间切换
 *  数字1:IF强拖结束并闭环转速环和观测器,减小Id给定进行收尾(IdIq电流环开启,转速环开启,观测器开启,角度:由观测器给定)
*/
uint8_t IF_Start_Step=0;
Q15_I_t IF_IqCurrent={0.0f};//IF强拖电流实际值(Iq给定值以每秒IF_IqCurrentAcceleration的速度增加,慢慢上升直到IF_IqCurrentTarget)
Q15_te_t IF_ETheta={0.0f};//IF角度
Q15_we_t IF_ESpeed={0.0f};//IF速度
Q15_te_t OB_ETheta={0.0f};//观测器角度
Q15_we_t OB_ESpeed={0.0f};//观测器速度

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern PCD_HandleTypeDef hpcd_USB_FS;
extern DMA_HandleTypeDef hdma_adc1;
extern ADC_HandleTypeDef hadc1;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M3 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f1xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles DMA1 channel1 global interrupt.
  */
void DMA1_Channel1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel1_IRQn 0 */

  /* USER CODE END DMA1_Channel1_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_adc1);
  /* USER CODE BEGIN DMA1_Channel1_IRQn 1 */

  /* USER CODE END DMA1_Channel1_IRQn 1 */
}
/**
  * @brief This function handles ADC1 and ADC2 global interrupts.
  */
void ADC1_2_IRQHandler(void)
{
  /* USER CODE BEGIN ADC1_2_IRQn 0 */

  /* USER CODE END ADC1_2_IRQn 0 */
  HAL_ADC_IRQHandler(&hadc1);
  /* USER CODE BEGIN ADC1_2_IRQn 1 */
  /*
   *获取ABC相电流
   */
  const int32_t va_source=(int32_t)HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1);
  const int32_t vb_source=(int32_t)HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_2);
  extern int16_t IA_REF;extern int16_t IB_REF;
  const Q15_I_t Ia_raw=ADC_GetCurrent_Q15(va_source,IA_REF);
  const Q15_I_t Ib_raw=ADC_GetCurrent_Q15(vb_source,IB_REF);
  const Q15_I_t Ia=lowPass_filter_Ia(Ia_raw);
  const Q15_I_t Ib=lowPass_filter_Ib(Ib_raw);
  const Q15_I_t Ic=Q15_C2C(Q15_add(_NP(Ia),_NP(Ib)),I);
  /*
  * 执行一次Clarke，获取静止三相电流
  */
  ClarkePark.clarke.Ia_I=Ia;
  ClarkePark.clarke.Ib_I=Ib;
  ClarkePark.clarke.Ic_I=Ic;
  Clarke_transform(&ClarkePark.clarke);

  Q15_we_t Espeed;
  Q15_te_t Etheta;

  static Q15_U_t Valpha_last={0};
  static Q15_U_t Vbeta_last={0};
  fluxObserver_pll_est.Ialpha_I=ClarkePark.clarke.Ialpha_O;
  fluxObserver_pll_est.Ibeta_I=ClarkePark.clarke.Ibeta_O;
  fluxObserver_pll_est.Valpha_I=Valpha_last;
  fluxObserver_pll_est.Vbeta_I=Vbeta_last;
  FluxObserver_PLL_update(&fluxObserver_pll_est);
  OB_ESpeed=fluxObserver_pll_est.Espeed_O;
  OB_ETheta=fluxObserver_pll_est.Etheta_O;

  if (IF_Start_Step==0) {
    IF_ESpeed=Q15_C2C(Q15_add(_P(IF_ESpeed),Q15_mul(_P(IF_Acceleration),_P(T_s))),we);
    IF_ETheta=Q15_C2C(Q15_add(_P(IF_ETheta),Q15_mul(_P(IF_ESpeed),Q15_gain(_P(T_s), 2.f))), te);
    Espeed=IF_ESpeed;
    Etheta=IF_ETheta;
    if (IF_ESpeed.child_value>=IF_Target_Speed.child_value) {
      //强拖过渡阶段，进行两次坐标变换切换闭环
      IF_Start_Step=1;
      ClarkePark.park.Ialpha_I=Id_PIstate.Set;
      ClarkePark.park.Ibeta_I=Iq_PIstate.Set;
      ClarkePark.park.Theta_I=Q15_C2C(Q15_sub(_P(OB_ETheta),_P(IF_ETheta)),te);
      Park_transform(&ClarkePark.park);
      Id_PIstate.Set=ClarkePark.park.Id_O;
      Iq_PIstate.Set=ClarkePark.park.Iq_O;

      ClarkePark.park.Ialpha_I=Q15_C2C(_P(Id_PIstate.Output),I);
      ClarkePark.park.Ibeta_I=Q15_C2C(_P(Iq_PIstate.Output),I);
      Park_transform(&ClarkePark.park);
      Id_PIstate.Output=Q15_C2C(_P(ClarkePark.park.Id_O),U);
      Iq_PIstate.Output=Q15_C2C(_P(ClarkePark.park.Iq_O),U);

      Id_PIstate.I_Output_Part=((int64_t)(Id_PIstate.Output.child_value))<<(15+Id_PIstate.I_RightMove);
      Iq_PIstate.I_Output_Part=((int64_t)(Iq_PIstate.Output.child_value))<<(15+Iq_PIstate.I_RightMove);

      Speed_PIstate.Measure=IF_ESpeed;
      Speed_PI_update(&Speed_PIstate);
      Speed_PIstate.Output=Iq_PIstate.Set;
      Speed_PIstate.I_Output_Part=((int64_t)(Speed_PIstate.Output.child_value-Speed_PIstate.P_Output_Part.child_value))<<(15+Speed_PIstate.I_RightMove);

      Etheta=OB_ETheta;
    }
  }
  else {
    Espeed=OB_ESpeed;
    Etheta=OB_ETheta;
  }

  /*
  * 执行一次转速环，获取Q电流环给定(转速环频率已降低为1khz)
  */
  static int SpeedCount=0;
  SpeedCount++;
  if (SpeedCount==10) {
    Speed_PIstate.Measure=Espeed;
    Speed_PI_update(&Speed_PIstate);
    SpeedCount=0;
  }
  /*
  * 执行一次Park，获取dq电流
  */
  ClarkePark.park.Ialpha_I=ClarkePark.clarke.Ialpha_O;
  ClarkePark.park.Ibeta_I=ClarkePark.clarke.Ibeta_O;
  ClarkePark.park.Theta_I=Etheta;
  Park_transform(&ClarkePark.park);

  /*
   * 执行一次dq电流环，获取Udq电压给定
   */
  if (Id_PIstate.Set.child_value>0)Id_PIstate.Set.child_value-=Q15_FromValue(0.0001f,I).child_value;
  else Id_PIstate.Set=Q15_FromValue(0.f,I);
  Id_PIstate.Measure=ClarkePark.park.Id_O;
  Id_PI_update(&Id_PIstate);
  if (IF_Start_Step==0) {
    if (IF_IqCurrent.child_value<IF_IqCurrentTarget.child_value)IF_IqCurrent=Q15_C2C(Q15_add(_P(IF_IqCurrent),Q15_mul(_P(IF_IqCurrentAcceleration),_P(T_s))),I);
    Iq_PIstate.Set=IF_IqCurrent;
  }
  else {
    Iq_PIstate.Set=Speed_PIstate.Output;
  }
  Iq_PIstate.Measure=ClarkePark.park.Iq_O;
  Iq_PI_update(&Iq_PIstate);
   /*
   * 执行反park前，将上次设置的Ualpha和Ubeta记录下来，用于下一次中断时的观测器预测
   */
   Valpha_last=ClarkePark.ipark.Valpha_O;
   Vbeta_last=ClarkePark.ipark.Vbeta_O;
  /*
   * 执行一次反park，获取Ualpha和Ubeta
   */
  ClarkePark.ipark.Vd_I=Id_PIstate.Output;
  ClarkePark.ipark.Vq_I=Iq_PIstate.Output;
  ClarkePark.ipark.Theta_I=Etheta;
  IPark_transform(&ClarkePark.ipark);

  /*
  * 对计算所得的矢量进行限幅(限制为电流环最大输出值)
  */
  const Q15_U_t V_modulus=Q15_C2C(Q15_module(_P(ClarkePark.ipark.Valpha_O),_P(ClarkePark.ipark.Vbeta_O)),U);

  if (V_modulus.child_value>Q15_FromValue(Current_MaxOutput,U).child_value) {
     ClarkePark.ipark.Valpha_O=(Q15_U_t){(int32_t)ClarkePark.ipark.Valpha_O.child_value*Q15_FromValue(Current_MaxOutput,U).child_value/V_modulus.child_value};
     ClarkePark.ipark.Vbeta_O=(Q15_U_t){(int32_t)ClarkePark.ipark.Vbeta_O.child_value*Q15_FromValue(Current_MaxOutput,U).child_value/V_modulus.child_value};
  }

  /*
   * 执行一次SVPWM，更新计数值
   */
  SVPWM_Calculate_Set(ClarkePark.ipark.Valpha_O,ClarkePark.ipark.Vbeta_O);
  //记录数据
  recordRunningData();
  /* USER CODE END ADC1_2_IRQn 1 */
}

/**
  * @brief This function handles USB low priority or CAN RX0 interrupts.
  */
void USB_LP_CAN1_RX0_IRQHandler(void)
{
  /* USER CODE BEGIN USB_LP_CAN1_RX0_IRQn 0 */

  /* USER CODE END USB_LP_CAN1_RX0_IRQn 0 */
  HAL_PCD_IRQHandler(&hpcd_USB_FS);
  /* USER CODE BEGIN USB_LP_CAN1_RX0_IRQn 1 */

  /* USER CODE END USB_LP_CAN1_RX0_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
