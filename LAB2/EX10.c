/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
const int MAX_LED = 4;
const uint8_t LED7_ANODE[10] = {
    0x40, 0x79, 0x24, 0x30, 0x19,
    0x12, 0x02, 0x78, 0x00, 0x10
};

void SetTimer(int duration, uint8_t timer);
int indexShift=0;
int index_led = 0;
int led_buffer [4] = {1 , 2 , 3 , 4};
int hour = 15 , minute = 8 , second = 50;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

void display7SEG(uint8_t num) {
   if (num<0 || num > 9) return;
   uint8_t code = LED7_ANODE[num];
   for (uint8_t i = 0; i < 7; i++) {
       HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 << i, (code >> i) & 0x01);
   }
}
void setEnable(uint8_t index, int total_leds) {
   for (int i = 0; i < total_leds; i++) {
       // Chân PA6, PA7, PA8, PA9 tương ứng cho các LED
       HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6 << i, (index == i) ? GPIO_PIN_RESET : GPIO_PIN_SET);
   }
}
void update7SEG ( int index ) {
	switch ( index ) {
		case 0:{
			// Display the first 7 SEG with led_buffer [0]
			setEnable(0,MAX_LED);
			display7SEG(led_buffer[0]);
			break ;
		}
		case 1:{
			// Display the second 7 SEG with led_buffer [1]
			setEnable(1,MAX_LED);
			display7SEG(led_buffer[1]);
			break ;
		}
		case 2:{
			// Display the third 7 SEG with led_buffer [2]
			setEnable(2,MAX_LED);
			display7SEG(led_buffer[2]);
			break ;
		}
		case 3:{
			// Display the forth 7 SEG with led_buffer [3]
			setEnable(3,MAX_LED);
			display7SEG(led_buffer[3]);
			break ;
		}
		default :
			break ;
	}
}
void updateClockBuffer(){
	led_buffer[0]= hour/10;
	led_buffer[1]= hour%10;
	led_buffer[2]= minute/10;
	led_buffer[3]= minute%10;
}

void displayRow(uint8_t value) {
    for (int i = 0; i < 8; i++) {
        // Bit = 1 -> Xuất SET (3.3V - �?�?) để LED S�?NG
        // Bit = 0 -> Xuất RESET (0V - Xanh) để LED TẮT
        GPIO_PinState state = (value  & (0x01<<(7-i)) ) ? GPIO_PIN_SET : GPIO_PIN_RESET;

        // PB8 tương ứng bit 0, PB9 tương ứng bit 1, ..., PB15 tương ứng bit 7
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8 << i, state);
    }
}
const int MAX_LED_MATRIX = 8;
int index_led_matrix = 0;

// Mã Hex hiển thị mẫu test hoặc chữ "A"
uint8_t matrix_buffer[16] = {0xE0, 0xCF, 0xAF, 0x6F, 0x6F, 0xAF, 0xCF, 0xE0,0xE0, 0xCF, 0xAF, 0x6F, 0x6F, 0xAF, 0xCF, 0xE0};
uint8_t matrix_buffer_B[16] ={0x0, 0x6E, 0x6E, 0x99, 0xFF, 0xFF,0xFF, 0xFF,0x0, 0x6E, 0x6E, 0x99, 0xFF, 0xFF,0xFF, 0xFF};



void updateLEDMatrix(int index) {
     //1. TẮT TẤT CẢ 8 CỘT 
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13
			  	  	  	  	  |GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_SET);

	// 2. NẠP DỮ LIỆU HÀNG CHO CỘT HIỆN TẠI
	displayRow(matrix_buffer[index+indexShift]);

	// 3. BẬT DUY NHẤT CỘT TƯƠNG ỨNG VỚI INDEX
	switch (index) {
		case 0:
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
			break;
		case 1:
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
			break;
		case 2:
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
			break;
		case 3:
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
			break;
		case 4:
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
			break;
		case 5:
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, GPIO_PIN_RESET);
			break;
		case 6:
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, GPIO_PIN_RESET);
			break;
		case 7:
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
			break;
		default:
			break;
	}
}

void handleShifft(){

	    indexShift++;
	    if (indexShift >= 8) {
	            indexShift = 0;
	    }
}
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

int counterSEG=0;
int counterLed=0;
int counterMatrix=0;
int counterShift=0;
uint8_t DO_PER_250ms=0;
uint8_t DO_Matrixms=0;
uint8_t DOShift=0;
uint8_t DO_PER_1000ms=0;
#define BaseTick 5
void TimerRun(){
	if(counterSEG>0){
		counterSEG--;
		if(counterSEG<=0){
			DO_PER_250ms=1;
		}
	}
	if(counterLed>0){
		counterLed--;
		if(counterLed<=0){
			DO_PER_1000ms=1;
		}
	}
	if(counterMatrix>0){
		counterMatrix--;
		if(counterMatrix<=0){
			DO_Matrixms=1;
		}
	}
	if(counterShift >0){
		counterShift--;
		if(counterShift <=0){
			DOShift=1;
		}
	}
}

void SetTimer(int duration, uint8_t timer){
	switch (timer) {
		case 0:{
			DO_PER_250ms=0;
			counterSEG=duration/BaseTick;
			break;
		}
		case 1:{
			DO_Matrixms=0;
			counterMatrix=duration/BaseTick;
			break;
		}
		case 2:{
			DO_PER_1000ms=0;
			counterLed=duration/BaseTick;
			break;
		}
		case 3:{
			DOShift=0;
			counterShift=duration/BaseTick;
			break;
		}
		default:
			break;
	}
}
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void HAL_TIM_PeriodElapsedCallback ( TIM_HandleTypeDef * htim )
{

	TimerRun();

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
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */

  	  HAL_TIM_Base_Start_IT(&htim2);
  	// 1. Cho LED 0 hiển thị ngay lập tức từ t = 0ms
	  update7SEG(index_led);
	  index_led = (index_led + 1) % MAX_LED;
	  updateClockBuffer();
  	  SetTimer(250,0);
  	  SetTimer(5,1);
  	  SetTimer(1000,2);
  	  SetTimer(200, 3);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1){

	 if(DO_PER_250ms==1){
		 SetTimer(250,0);
			 update7SEG(index_led);
			 index_led=(index_led+1)%MAX_LED;
	 }

	 if(DO_PER_1000ms==1){
		 SetTimer(1000,2);
		 second ++;
		  if ( second >= 60) {
			   second = 0;
			   minute ++;
		  }
		  if( minute >= 60) {
			   minute = 0;
			   hour ++;
		  }
		 if( hour >=24) {
			   hour = 0;
		   }
		 updateClockBuffer();
		 HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_4);
	}
	 if(DO_Matrixms==1){
		    SetTimer(5,1);
			updateLEDMatrix(index_led_matrix);
			index_led_matrix =(index_led_matrix+1)% MAX_LED_MATRIX;
	 }
	 if(DOShift==1){
		 SetTimer(200,3); // Đặt lại timer 200ms
		 handleShifft();
	 }
  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
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
  htim2.Init.Prescaler = 7999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 4;
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
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9
                          |GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13
                          |GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_10
                          |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14
                          |GPIO_PIN_15|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6|GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA2 PA3 PA4 PA5
                           PA6 PA7 PA8 PA9
                           PA10 PA11 PA12 PA13
                           PA14 PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9
                          |GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13
                          |GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB2 PB10
                           PB11 PB12 PB13 PB14
                           PB15 PB3 PB4 PB5
                           PB6 PB8 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_10
                          |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14
                          |GPIO_PIN_15|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
