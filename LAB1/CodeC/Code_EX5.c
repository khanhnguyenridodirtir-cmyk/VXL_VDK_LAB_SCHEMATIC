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

/* USER CODE END PTD */

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
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
typedef struct {
    GPIO_TypeDef* port;
    uint16_t pin;
} LED_Pin;

typedef struct {
    LED_Pin led1;
    LED_Pin led2;
    LED_Pin led3;
} TripleLED;

void set_single_led(const TripleLED *leds, uint8_t active_led) {

    HAL_GPIO_WritePin(leds->led1.port, leds->led1.pin, (active_led == 1) ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(leds->led2.port, leds->led2.pin, (active_led == 2) ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(leds->led3.port, leds->led3.pin, (active_led == 3) ? GPIO_PIN_RESET : GPIO_PIN_SET);
}
void display7SEG(int num) {
    if (num < 0 || num > 9) return;

    const uint8_t seg_code[10] = {
        0x40, 0x79, 0x24, 0x30, 0x19,
        0x12, 0x02, 0x78, 0x00, 0x10
    };

    // Dùng vòng lặp quét từng bit từ PB0 đến PB6
    for (int i = 0; i < 7; i++) {
        GPIO_PinState state = ((seg_code[num] >> i) & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET;
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1 << i, state);
    }
}
void countdown(int* counter, int num){
	while(*counter >=num){
		 display7SEG(*counter);
		  *counter=*counter-1;

		  HAL_Delay(900);
	}
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
  /* USER CODE BEGIN 2 */
  TripleLED my_leds1 = {
        .led1 = {Led_RED1_GPIO_Port, Led_RED1_Pin},
        .led2 = {GPIOA, Led_YELLOW1_Pin},
        .led3 = {GPIOA, Led_GREEN1_Pin}
    };
    TripleLED my_leds2 = {
        .led1 = {Led_RED2_GPIO_Port, Led_RED2_Pin},
        .led2 = {GPIOA, Led_YELLOW2_Pin},
        .led3 = {GPIOA, Led_GREEN2_Pin}
    };
    set_single_led(&my_leds1, 0);
     set_single_led(&my_leds2, 0);

     const int RED=5;
     const int Green=3;
     const int YELLOW=2;

     int counter =RED;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  set_single_led(&my_leds1, 1);
	  set_single_led(&my_leds2,3);
	  countdown(&counter,3);

	  set_single_led(&my_leds2, 2);

	  countdown(&counter,0);
	  counter=Green;

	  set_single_led(&my_leds1, 3);
	  set_single_led(&my_leds2, 1);

	  countdown(&counter,0);
	  counter=YELLOW;

	  set_single_led(&my_leds1, 2);
	  countdown(&counter,0);
	  counter=RED;
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
  HAL_GPIO_WritePin(GPIOA, Led_RED1_Pin|Led_YELLOW1_Pin|Led_GREEN1_Pin|Led_RED2_Pin
                          |Led_YELLOW2_Pin|Led_GREEN2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, Seg7_A_Pin|Seg7_B_Pin|Seg7_C_Pin|Seg7_D_Pin
                          |Seg7_E_Pin|Seg7_F_Pin|Seg7_G_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : Led_RED1_Pin Led_YELLOW1_Pin Led_GREEN1_Pin Led_RED2_Pin
                           Led_YELLOW2_Pin Led_GREEN2_Pin */
  GPIO_InitStruct.Pin = Led_RED1_Pin|Led_YELLOW1_Pin|Led_GREEN1_Pin|Led_RED2_Pin
                          |Led_YELLOW2_Pin|Led_GREEN2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : Seg7_A_Pin Seg7_B_Pin Seg7_C_Pin Seg7_D_Pin
                           Seg7_E_Pin Seg7_F_Pin Seg7_G_Pin */
  GPIO_InitStruct.Pin = Seg7_A_Pin|Seg7_B_Pin|Seg7_C_Pin|Seg7_D_Pin
                          |Seg7_E_Pin|Seg7_F_Pin|Seg7_G_Pin;
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
