/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f1xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "fixed_point.h"
#include "PMSM_Control_Core/Hardware.h"
#include "PMSM_Control_Core/Clarke_Park.h"
#include "PMSM_Control_Core/EKF.h"
#include "PMSM_Control_Core/PI_Controller.h"
#include "PMSM_Control_Core/SVPWM.h"
#include "USB_JustFloat.h"
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
  extern volatile uint32_t timerFlag;
  if (TimeMeasure) {
    if (timerFlag<USB_HalfDataCount) {
      timerFlag++;
    }
    if (timerFlag==USB_HalfDataCount) {
      CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
      DWT->CYCCNT = 0;
      DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    }
    if (USB_HalfDataCount%15!=0) {
      //额外执行一次转速环，以更准确计算全部代码消耗的时间
      Speed_PIstate.Measure=ekf_est.Espeed_O;
      Speed_PI_update(&Speed_PIstate);
    }
  }
  //上述代码仅用于测试该函数执行一次的时间
  /*
  性能测试：执行时间:3277周期，合45.5us
  */

  /*
   *获取ABC相电流
   */
  const int32_t va_source=(int32_t)HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1);
  const int32_t vb_source=(int32_t)HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_2);
  const R15_t va_source_R=(R15_t){va_source<<(31-15)};
  const R15_t vb_source_R=(R15_t){vb_source<<(31-15)};
  const R2_t Ia_raw=R15_0_2_mul(R15_sub(IA_REF_R,va_source_R),IA_K_4095_inv_VCC_3V3);
  const R2_t Ib_raw=R15_0_2_mul(R15_sub(IB_REF_R,vb_source_R),IB_K_4095_inv_VCC_3V3);
  const R2_t Ia=median_filter_Ia_5(Ia_raw);
  const R2_t Ib=median_filter_Ib_5(Ib_raw);
  const R2_t Ic=R2_neg(R2_add(Ia,Ib));
  /*
  * 执行一次Clarke，获取静止三相电流
  */
  ClarkePark.clarke.Ia_I=Ia;
  ClarkePark.clarke.Ib_I=Ib;
  ClarkePark.clarke.Ic_I=Ic;
  Clarke_transform(&ClarkePark.clarke);

  /*
   * 执行一次EKF,获取转子角度和速度
   * 注意这里输入电压取上一次中断时计算的电压(事实上应当为上上次中断时计算的电压)
   * 因为这次计算的电压，将会在下个周期作用
   */
  ekf_est.Ialpha_I=ClarkePark.clarke.Ialpha_O;
  ekf_est.Ibeta_I=ClarkePark.clarke.Ibeta_O;
  ekf_est.Valpha_I=ClarkePark.ipark.Valpha_O;
  ekf_est.Vbeta_I=ClarkePark.ipark.Vbeta_O;
  EKF_update(&ekf_est);

  /*
  * 执行一次转速环，获取Q电流环给定(转速环频率已降低为1khz)
  */
  static int SpeedCount=0;
  SpeedCount++;
  if (SpeedCount==15) {
    Speed_PIstate.Measure=ekf_est.Espeed_O;
    Speed_PI_update(&Speed_PIstate);
    SpeedCount=0;
  }


  /*
  * 执行一次Park，获取dq电流
  */
  ClarkePark.park.Ialpha_I=ClarkePark.clarke.Ialpha_O;
  ClarkePark.park.Ibeta_I=ClarkePark.clarke.Ibeta_O;
  ClarkePark.park.Theta_I=ekf_est.Etheta_O;
  Park_transform(&ClarkePark.park);

  /*
   * 执行一次dq电流环，获取Udq电压给定
   */
  Id_PIstate.Measure=ClarkePark.park.Id_O;
  Id_PI_update(&Id_PIstate);
  Iq_PIstate.Set=Speed_PIstate.Output;
  Iq_PIstate.Measure=ClarkePark.park.Iq_O;
  Iq_PI_update(&Iq_PIstate);

  /*
   * 执行一次反park，获取Ualpha和Ubeta
   */
  ClarkePark.ipark.Vd_I=Id_PIstate.Output;
  ClarkePark.ipark.Vq_I=Iq_PIstate.Output;

  ClarkePark.ipark.Theta_I=ekf_est.Etheta_O;
  IPark_transform(&ClarkePark.ipark);

  /*
  * 对计算所得的矢量进行限幅(6.5V)
  */
  const R8_t modulus_sq=R8_add(R4_4_8_mul(ClarkePark.ipark.Valpha_O,ClarkePark.ipark.Valpha_O)
     ,R4_4_8_mul(ClarkePark.ipark.Vbeta_O,ClarkePark.ipark.Vbeta_O));
  const R8_t modulus=R8_sqrt(modulus_sq);
  const R0_t modulus_inv=R8_inv(modulus);
  const R0_t gain=R0_f4_0_mul(modulus_inv,6.5f);
  if (R8_float_greater(modulus,6.5f)) {
    ClarkePark.ipark.Valpha_O=R4_0_4_mul(ClarkePark.ipark.Valpha_O,gain);
    ClarkePark.ipark.Vbeta_O=R4_0_4_mul(ClarkePark.ipark.Vbeta_O,gain);
  }
  /*
  * 执行一次SVPWM，更新计数值
  */
  SVPWM_Calculate_Set(ClarkePark.ipark.Valpha_O,ClarkePark.ipark.Vbeta_O);

  recordRunningData();
  if (TimeMeasure) {
    if (timerFlag==USB_HalfDataCount) {
      timerFlag=DWT->CYCCNT;
    }
    //上述代码仅用于测试该函数执行一次的时间
  }
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
