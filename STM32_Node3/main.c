/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DRY_VALUE 3358
#define WET_VALUE 1360

#define NODE_ID 3// 🔴 CHANGE THIS for each node
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM2_Init(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* -------- LoRa Write -------- */
void LoRa_Write(uint8_t addr, uint8_t data)
{
    uint8_t buf[2];
    buf[0] = addr | 0x80;
    buf[1] = data;

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, buf, 2, 100);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
}

void LoRa_Write_Buffer(uint8_t addr, uint8_t *data, uint8_t len)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);

    uint8_t addrByte = addr | 0x80;
    HAL_SPI_Transmit(&hspi1, &addrByte, 1, 100);
    HAL_SPI_Transmit(&hspi1, data, len, 100);

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
}

/* -------- Delay us -------- */
void delay_us(uint16_t us)
{
    __HAL_TIM_SET_COUNTER(&htim2, 0);
    while (__HAL_TIM_GET_COUNTER(&htim2) < us);
}

/* -------- DHT11 Read -------- */
uint8_t DHT11_Read(void)
{
    uint8_t i = 0, j;
    for (j = 0; j < 8; j++)
    {
        uint32_t timeout = 0;
        while (!(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1)))
            if (++timeout > 10000) return 0;

        delay_us(40);

        if (!(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1)))
            i &= ~(1 << (7 - j));
        else
        {
            i |= (1 << (7 - j));
            timeout = 0;
            while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1))
                if (++timeout > 10000) break;
        }
    }
    return i;
}

void Enter_Stop_Mode(void)
{
    HAL_SuspendTick();

    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

    HAL_ResumeTick();
    SystemClock_Config(); // MUST after wakeup
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

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
  MX_SPI1_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_RTC_Init();
  HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start(&htim2);

  srand(HAL_GetTick());

  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
  HAL_Delay(10);

  // RESET LoRa
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
  HAL_Delay(10);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
  HAL_Delay(10);

  // LoRa setup
  LoRa_Write(0x01, 0x00);
  HAL_Delay(10);
  LoRa_Write(0x01, 0x80);
  HAL_Delay(10);

  LoRa_Write(0x1D, 0x72);
  LoRa_Write(0x1E, 0x74 | 0x04);
  LoRa_Write(0x26, 0x04);
  LoRa_Write(0x23, 0xFF);
  LoRa_Write(0x31, 0x43);
  LoRa_Write(0x37, 0x0A);
  LoRa_Write(0x39, 0x34);

  LoRa_Write(0x20, 0x00);
  LoRa_Write(0x21, 0x08);

  LoRa_Write(0x06, 0x6C);
  LoRa_Write(0x07, 0x40);
  LoRa_Write(0x08, 0x00);

  LoRa_Write(0x09, 0x8F);
  LoRa_Write(0x0C, 0x23);

  LoRa_Write(0x0E, 0x00);
  LoRa_Write(0x0F, 0x00);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  // -------- INITIAL OFFSET (only once) --------
  if (NODE_ID == 2) HAL_Delay(6000);
  if (NODE_ID == 3) HAL_Delay(12000);
  while (1)
  {



      GPIO_InitTypeDef GPIO_InitStruct = {0};


      uint8_t rain_status;
      uint8_t Rh_byte1, Rh_byte2, Temp_byte1, Temp_byte2, checksum;
      uint32_t timeout;
      char msg[100];


      HAL_Delay(1000); // DHT stabilize after wake

      // -------- DHT START --------
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
      HAL_Delay(18);
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
      delay_us(20);

      GPIO_InitStruct.Pin = GPIO_PIN_1;
      GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
      HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

      timeout = 0;
      while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) && ++timeout < 10000);
      timeout = 0;
      while (!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) && ++timeout < 10000);
      timeout = 0;
      while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) && ++timeout < 10000);

      Rh_byte1 = DHT11_Read();
      Rh_byte2 = DHT11_Read();
      Temp_byte1 = DHT11_Read();
      Temp_byte2 = DHT11_Read();
      checksum = DHT11_Read();

      // -------- ADC (Soil) --------
      uint32_t adc_sum = 0;
      for(int i = 0; i < 10; i++)
      {
          HAL_ADC_Start(&hadc1);
          HAL_ADC_PollForConversion(&hadc1, 10);
          adc_sum += HAL_ADC_GetValue(&hadc1);
          HAL_ADC_Stop(&hadc1);
      }

      uint32_t adc_val = adc_sum / 10;

      int32_t moisture = (DRY_VALUE - adc_val) * 100 / (DRY_VALUE - WET_VALUE);
      if (moisture > 100) moisture = 100;
      if (moisture < 0) moisture = 0;

      rain_status = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);

      // -------- DHT RETRY --------
      if (checksum != (Rh_byte1 + Rh_byte2 + Temp_byte1 + Temp_byte2))
      {
          HAL_Delay(1000);

          HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
          HAL_Delay(18);
          HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
          delay_us(20);

          GPIO_InitStruct.Pin = GPIO_PIN_1;
          GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
          HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

          timeout = 0;
          while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) && ++timeout < 10000);
          timeout = 0;
          while (!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) && ++timeout < 10000);
          timeout = 0;
          while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) && ++timeout < 10000);

          Rh_byte1 = DHT11_Read();
          Rh_byte2 = DHT11_Read();
          Temp_byte1 = DHT11_Read();
          Temp_byte2 = DHT11_Read();
          checksum = DHT11_Read();
      }

      // -------- FORMAT --------
      if (checksum == (Rh_byte1 + Rh_byte2 + Temp_byte1 + Temp_byte2))
      {
          sprintf(msg, "Node %d: T:%d H:%d S:%ld R:%d",
                  NODE_ID, Temp_byte1, Rh_byte1, moisture,
                  (rain_status == 0) ? 1 : 0);
      }
      else
      {
          sprintf(msg, "Node %d: DHT_ERR S:%ld R:%d",
                  NODE_ID, moisture,
                  (rain_status == 0) ? 1 : 0);
      }

      // -------- LORA TX --------
      LoRa_Write(0x01, 0x81); // standby

      LoRa_Write(0x0E, 0x00);
      LoRa_Write(0x0D, 0x00);
      LoRa_Write(0x12, 0xFF);

      LoRa_Write_Buffer(0x00, (uint8_t*)msg, strlen(msg));
      LoRa_Write(0x22, strlen(msg));

      LoRa_Write(0x01, 0x83); // TX

      // -------- UART DEBUG --------
      HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);
      HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n", 2, 100);

      // -------- RESET DHT PIN --------
      GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
      HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);

      // -------- LORA SLEEP --------
      LoRa_Write(0x01, 0x80);

      // -------- BEFORE SLEEP --------
      char sleep_msg[] = ">>> ENTERING STOP MODE <<<\r\n";
      HAL_UART_Transmit(&huart2, (uint8_t*)sleep_msg, strlen(sleep_msg), 100);

      // ✅ ensure UART finished
      while(__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC) == RESET);

      // -------- ENTER STOP MODE --------
      Enter_Stop_Mode();

      // -------- AFTER WAKE --------
      char wake_msg[] = ">>> WOKE UP FROM STOP MODE <<<\r\n";
      HAL_UART_Transmit(&huart2, (uint8_t*)wake_msg, strlen(wake_msg), 100);

      // Re-init peripherals
      MX_SPI1_Init();
      HAL_Delay(5);

      // Re-init LoRa (minimal)
      LoRa_Write(0x01, 0x80); // sleep
      HAL_Delay(10);
      LoRa_Write(0x01, 0x81);
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
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

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 0x1;
  sDate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable the WakeUp
  */
  uint32_t wake_time = 60;

  if (NODE_ID == 2) wake_time = 66;
  if (NODE_ID == 3) wake_time = 72;

  if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, wake_time, RTC_WAKEUPCLOCK_CK_SPRE_16BITS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 83;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 0xFFFFFFFF;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PA1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PC7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
    // Wakeup interrupt — no code needed
}
/* USER CODE END 4 */

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
#ifdef USE_FULL_ASSERT
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
