/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "cmsis_os.h"
#include "touchsensing.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "tsl_user.h"
#include "drv2605l.h"
#include "mc3416.h"
#include <stdbool.h>
#include "math.h"
#include "stdlib.h"
#include "led.h"
#include "slider.h"
#include "bm71.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum {
	NONE,
	MODE,
	AUTODIM,
	POWER
} button_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c2;

TSC_HandleTypeDef htsc;

UART_HandleTypeDef huart5;

/* Definitions for mainTask */
osThreadId_t mainTaskHandle;
const osThreadAttr_t mainTask_attributes = {
  .name = "mainTask",
  .stack_size = 300 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for buttonTask */
osThreadId_t buttonTaskHandle;
const osThreadAttr_t buttonTask_attributes = {
  .name = "buttonTask",
  .stack_size = 64 * 4,
  .priority = (osPriority_t) osPriorityNormal5,
};
/* Definitions for uartTask */
osThreadId_t uartTaskHandle;
const osThreadAttr_t uartTask_attributes = {
  .name = "uartTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal3,
};
/* Definitions for I2C2_lock */
osMutexId_t I2C2_lockHandle;
const osMutexAttr_t I2C2_lock_attributes = {
  .name = "I2C2_lock"
};
/* Definitions for blueLedLock */
osMutexId_t blueLedLockHandle;
const osMutexAttr_t blueLedLock_attributes = {
  .name = "blueLedLock"
};
/* Definitions for bm71Lock */
osMutexId_t bm71LockHandle;
const osMutexAttr_t bm71Lock_attributes = {
  .name = "bm71Lock"
};
/* Definitions for buttonPressSem */
osSemaphoreId_t buttonPressSemHandle;
const osSemaphoreAttr_t buttonPressSem_attributes = {
  .name = "buttonPressSem"
};
/* Definitions for uartRecSem */
osSemaphoreId_t uartRecSemHandle;
const osSemaphoreAttr_t uartRecSem_attributes = {
  .name = "uartRecSem"
};
/* USER CODE BEGIN PV */
uint32_t currentTime;
uint32_t previousTime = 0;
volatile uint32_t lastActivity = 0;
uint32_t lastPwrPress = 0;
uint32_t lastModePress = 0;
uint32_t lastAutoDimPress = 0;
uint32_t lastMotion = 0;
uint32_t bleResetTime = 0;
uint32_t lastBattCheck = 0;
volatile bool modePressed = false;
volatile bool autoDimPresssed = false;
volatile bool pwrPressed = false;
volatile button_t buttonPressed = NONE; // in future, use flag/mask. for now this is fine as button presses are sparse
volatile bool toggleBlueActive;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TSC_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C2_Init(void);
static void MX_UART5_Init(void);
void StartMainTask(void *argument);
void StartButtonTask(void *argument);
void StartUartRecTask(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
//	printf("here!\n"); // avoid printf in RTOS
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */


  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TSC_Init();
  MX_ADC1_Init();
  MX_I2C2_Init();
  MX_TOUCHSENSING_Init();
  MX_UART5_Init();
  /* USER CODE BEGIN 2 */

  // ---------- USER PERIPHERAL INIT BEGIN ------------
  BM71_Init(&huart5);
  DRV2605L_Init(&hi2c2);
  DRV2605L_SequenceWaveform(7, 1);
  MC3416_Init(&hi2c2);
  // ---------- USER PERIPHERAL INIT END ------------

//  DRV2605L_SequenceWaveform(7, 1);
//  BM71_ReadDeviceName();
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();
  /* Create the mutex(es) */
  /* creation of I2C2_lock */
  I2C2_lockHandle = osMutexNew(&I2C2_lock_attributes);

  /* creation of blueLedLock */
  blueLedLockHandle = osMutexNew(&blueLedLock_attributes);

  /* creation of bm71Lock */
  bm71LockHandle = osMutexNew(&bm71Lock_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of buttonPressSem */
  buttonPressSemHandle = osSemaphoreNew(1, 0, &buttonPressSem_attributes);

  /* creation of uartRecSem */
  uartRecSemHandle = osSemaphoreNew(1, 0, &uartRecSem_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of mainTask */
  mainTaskHandle = osThreadNew(StartMainTask, NULL, &mainTask_attributes);

  /* creation of buttonTask */
  buttonTaskHandle = osThreadNew(StartButtonTask, NULL, &buttonTask_attributes);

  /* creation of uartTask */
  uartTaskHandle = osThreadNew(StartUartRecTask, NULL, &uartTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_UART5|RCC_PERIPHCLK_I2C2
                              |RCC_PERIPHCLK_ADC12;
  PeriphClkInit.Uart5ClockSelection = RCC_UART5CLKSOURCE_PCLK1;
  PeriphClkInit.Adc12ClockSelection = RCC_ADC12PLLCLK_DIV1;
  PeriphClkInit.I2c2ClockSelection = RCC_I2C2CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_MultiModeTypeDef multimode = {0};
  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the ADC multi-mode
  */
  multimode.Mode = ADC_MODE_INDEPENDENT;
  if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.SamplingTime = ADC_SAMPLETIME_601CYCLES_5;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0x2000090E;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief TSC Initialization Function
  * @param None
  * @retval None
  */
static void MX_TSC_Init(void)
{

  /* USER CODE BEGIN TSC_Init 0 */

  /* USER CODE END TSC_Init 0 */

  /* USER CODE BEGIN TSC_Init 1 */

  /* USER CODE END TSC_Init 1 */

  /** Configure the TSC peripheral
  */
  htsc.Instance = TSC;
  htsc.Init.CTPulseHighLength = TSC_CTPH_4CYCLES;
  htsc.Init.CTPulseLowLength = TSC_CTPL_4CYCLES;
  htsc.Init.SpreadSpectrum = DISABLE;
  htsc.Init.SpreadSpectrumDeviation = 1;
  htsc.Init.SpreadSpectrumPrescaler = TSC_SS_PRESC_DIV1;
  htsc.Init.PulseGeneratorPrescaler = TSC_PG_PRESC_DIV4;
  htsc.Init.MaxCountValue = TSC_MCV_8191;
  htsc.Init.IODefaultMode = TSC_IODEF_OUT_PP_LOW;
  htsc.Init.SynchroPinPolarity = TSC_SYNC_POLARITY_FALLING;
  htsc.Init.AcquisitionMode = TSC_ACQ_MODE_NORMAL;
  htsc.Init.MaxCountInterrupt = DISABLE;
  htsc.Init.ChannelIOs = TSC_GROUP2_IO4|TSC_GROUP3_IO4|TSC_GROUP4_IO2|TSC_GROUP5_IO2
                    |TSC_GROUP6_IO4;
  htsc.Init.ShieldIOs = 0;
  htsc.Init.SamplingIOs = TSC_GROUP2_IO3|TSC_GROUP3_IO3|TSC_GROUP4_IO1|TSC_GROUP5_IO3
                    |TSC_GROUP6_IO3;
  if (HAL_TSC_Init(&htsc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TSC_Init 2 */

  /* USER CODE END TSC_Init 2 */

}

/**
  * @brief UART5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART5_Init(void)
{

  /* USER CODE BEGIN UART5_Init 0 */

  /* USER CODE END UART5_Init 0 */

  /* USER CODE BEGIN UART5_Init 1 */

  /* USER CODE END UART5_Init 1 */
  huart5.Instance = UART5;
  huart5.Init.BaudRate = 115200;
  huart5.Init.WordLength = UART_WORDLENGTH_8B;
  huart5.Init.StopBits = UART_STOPBITS_1;
  huart5.Init.Parity = UART_PARITY_NONE;
  huart5.Init.Mode = UART_MODE_TX_RX;
  huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart5.Init.OverSampling = UART_OVERSAMPLING_16;
  huart5.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart5.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART5_Init 2 */

  /* USER CODE END UART5_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(UART_RX_IND_GPIO_Port, UART_RX_IND_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LRA_TRIG_GPIO_Port, LRA_TRIG_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LRA_EN_GPIO_Port, LRA_EN_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, BUTTN_PRESS_LED_Pin|LOW_BATT_LED_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(BLE_MODE_GPIO_Port, BLE_MODE_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(BLE_RST_N_GPIO_Port, BLE_RST_N_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : PC13 PC14 PC0 PC1
                           PC2 PC3 PC4 PC5
                           PC6 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_0|GPIO_PIN_1
                          |GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : UART_RX_IND_Pin */
  GPIO_InitStruct.Pin = UART_RX_IND_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(UART_RX_IND_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PA0 PA2 PA4 PA5
                           PA8 PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_8|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LRA_TRIG_Pin BUTTN_PRESS_LED_Pin LOW_BATT_LED_Pin */
  GPIO_InitStruct.Pin = LRA_TRIG_Pin|BUTTN_PRESS_LED_Pin|LOW_BATT_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : LRA_EN_Pin */
  GPIO_InitStruct.Pin = LRA_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LRA_EN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : ACC_INT_Pin */
  GPIO_InitStruct.Pin = ACC_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ACC_INT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PB10 PB11 PB12 PB15
                           PB5 PB7 */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_15
                          |GPIO_PIN_5|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : AUTO_DIM_Pin PWR_BUTTON_Pin MODE_BUTTON_Pin */
  GPIO_InitStruct.Pin = AUTO_DIM_Pin|PWR_BUTTON_Pin|MODE_BUTTON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : BLE_MODE_Pin */
  GPIO_InitStruct.Pin = BLE_MODE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BLE_MODE_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BLE_STATUS1_IND_Pin */
  GPIO_InitStruct.Pin = BLE_STATUS1_IND_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BLE_STATUS1_IND_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BLE_RST_N_Pin */
  GPIO_InitStruct.Pin = BLE_RST_N_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BLE_RST_N_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BLE_STATUS2_IND_Pin */
  GPIO_InitStruct.Pin = BLE_STATUS2_IND_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BLE_STATUS2_IND_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
int _write(int file, char *ptr, int len)
{
  (void)file;
  int DataIdx;

  for (DataIdx = 0; DataIdx < len; DataIdx++)
  {
    ITM_SendChar(*ptr++);
  }
  return len;
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	switch(GPIO_Pin)
	{
	case PWR_BUTTON_Pin:
		// reset IDLE timer.
		if (HAL_GetTick() - lastPwrPress > 200) {;
			buttonPressed = POWER;
			lastPwrPress = lastActivity = HAL_GetTick();
			osSemaphoreRelease(buttonPressSemHandle);
		}
		// blink BTTN PRESS LED Twice
		// send UART Command to BLE
		break;
	case MODE_BUTTON_Pin:
		if (HAL_GetTick() - lastModePress > 200) {;
			buttonPressed = MODE;
			lastModePress = lastActivity = HAL_GetTick();
			osSemaphoreRelease(buttonPressSemHandle);
		}
		// blink BTTN PRESS LED Twice
		// send UART Command to BLE
		break;
	case AUTO_DIM_Pin:
		if (HAL_GetTick() - lastAutoDimPress > 200) {;
			buttonPressed = AUTODIM;
			lastAutoDimPress = lastActivity = HAL_GetTick();
			osSemaphoreRelease(buttonPressSemHandle);
		}
		// blink BTTN PRESS LED Twice
		// send UART Command to BLE
		break;
	case ACC_INT_Pin:
//		printf("acc_int_ INTERRUPT!\n"); // avoid printf in RTOS
		if (HAL_GetTick() - lastMotion > 200) {
			lastMotion = lastActivity = HAL_GetTick();
		}
		// send UART Command to BLE
		break;

	}
}

/*
 * Receives bm71 UART responses in background.
 *
 * Although unlikely, rxBuf and rxBufSize could IN THEORY be overwritten by this interrupt
 * while a previous bm71_ParseResponse is running.
 *
 * In future, if this actually becomes an issue, use some sort of circular buffer to
 * avoid contention over the RxBuf and its size.
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
  if (BM71_GetHandle() == huart) {
	  BM71_SetRxSize(Size);
	  HAL_UARTEx_ReceiveToIdle_IT(BM71_GetHandle(), BM71_GetRxBuf(), 256); // lthough unlikely, rxBuf in theory could be overwritten with bm71 asynch response while in parse function. deal with later, insignificant issue.
	  osSemaphoreRelease(uartRecSemHandle);
  }
}

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartMainTask */
/**
  * @brief  Function implementing the mainTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartMainTask */
void StartMainTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {

	  if (HAL_GetTick() - lastActivity >= 10000) {

		  	  // sleep user peripherals
		  	  osMutexAcquire(bm71LockHandle, osWaitForever); // wait to avoid sleeping if separate task is modifying bm71 state
			  BM71_Sleep();
			  DRV2605L_Disable();
			  TurnOffAllLED();

			  // disable clocks
			  HAL_SuspendTick();
			  SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk; // temporary (else RTOS tick wakes us up)

			  // enter stop mode
			  HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFI);

			  // enable clocks
			  SystemClock_Config();
			  SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk; // temporary (else RTOS tick wakes us up)
			  HAL_ResumeTick();

			  // wake user peripherals & notify user
			  TurnOnBlueLED();
			  BM71_Wake();
		  	  osMutexRelease(bm71LockHandle); // wait JUST IN CASE a task is affecting internal bm71 state. (will likely never happen).
			  DRV2605L_Enable();
			  DRV2605L_TriggerHaptics(); // needs 250us to accept i2c commands.
			  lastActivity = HAL_GetTick(); // does not include time spent in STOP. fine for our use case.
		  }

		  // check TSC (touch) readings
		  if (tsl_user_Exec() != TSL_USER_STATUS_BUSY) {
		      ProcessTouch();
		  }


	//	  if (bm71_GetStatus() == STANDBY && !bm71_ConnectionPending()) {
	//		  bm71_ConnectToDisplay();
	//	  }

		  // check battery status (could do extra task for this, but RAM usage limited)
		  uint32_t currTime = HAL_GetTick();
		  if (currTime - lastBattCheck > 2000) { // abstract away this timing function into the led driver
			  CheckLowBatt(&hadc1);
			  lastBattCheck = currTime;
		  }
    osDelay(10); // TSC doesn't work properly without some sort of delay. fix later
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartButtonTask */
/**
* @brief Function implementing the buttonTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartButtonTask */
void StartButtonTask(void *argument)
{
  /* USER CODE BEGIN StartButtonTask */
  /* Infinite loop */
  for(;;)
  {
	  // wakes on button press GPIO/EXTI interrupt releasing semaphore.
  	  osSemaphoreAcquire(buttonPressSemHandle, osWaitForever);
  	  osMutexAcquire(bm71LockHandle, osWaitForever); // wait if another task is affecting internal state of bm71
	  if (buttonPressed != NONE) {
		  switch (buttonPressed) { // could make bitmask if dealing  with multiple presses. but presses are fast and sparse. not needed now.
			case MODE:
				BM71_CommandDisplay(COMMAND, 0);
				break;
			case AUTODIM:
				BM71_CommandDisplay(COMMAND, 1); //
				break;
			case POWER:
				BM71_CommandDisplay(COMMAND, 2);//
				break;
			default:
				break;
		}
		osMutexRelease(bm71LockHandle);
		  buttonPressed = NONE;

		  // could implement in separate task, but presses are sparse. works for now, change later if needed.
		  TurnOnBlueLED();
		  osDelay(50); // task sleep 50ms

		  TurnOffBlueLED();
		  osDelay(50);

		  TurnOnBlueLED();
		  osDelay(50);

		  TurnOffBlueLED();
	  }
  }
  /* USER CODE END StartButtonTask */
}

/* USER CODE BEGIN Header_StartUartRecTask */
/**
* @brief Function implementing the uartTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartUartRecTask */
void StartUartRecTask(void *argument)
{
  /* USER CODE BEGIN StartUartRecTask */
  /* Infinite loop */
  for(;;)
  {
	// wakes on rxEventISR to releasing semaphore.
	// not too much different from rxReady signal we had in superloop.
	osSemaphoreAcquire(uartRecSemHandle, osWaitForever);
	osMutexAcquire(bm71LockHandle, osWaitForever); // wait if another task is affecting internal state of bm71
    BM71_ParseResponse();
    osMutexRelease(bm71LockHandle);
  }
  /* USER CODE END StartUartRecTask */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
