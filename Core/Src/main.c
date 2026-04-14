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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32f411xe.h"
#include "stdint.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define RCC_GPIOC_CLK_ENABLE (1U << 2) // Define MACRO to Enabling Clock for GPIOC
#define GPIOC_PC14_PC15_MODER_CLEAR ~((3U << 28) | (3U << 30)) // Define MACRO to CLEAR the bits in MODER14 - 15
#define GPIOC_PC14_PC15_OUTPUT_MODE ((1U << 28) | (1U << 30)) // Define MACRO to SET the bit in MODER14 - 15

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile uint8_t LED_STATE = 0;
volatile uint32_t current_millis = 0; // Variable global for store time
volatile uint32_t last_exti_time = 0; // Store the last time button pressed
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void EXTI0_IRQHandler(void) {
	// 1. Make sure the trigger is line 0
	if (EXTI->PR & (1U << 0)) {

		// SOFTWARE DEBOUNCING LOGIC
		if ((current_millis - last_exti_time) > 50) {
			// 2. Toggle the LED
			LED_STATE = !LED_STATE;
			if (LED_STATE) {
				GPIOC->BSRR = (1U << 15);
			} else {
				GPIOC->BSRR = (1U << (15 + 16));
			}

			// Update the last time process
			last_exti_time = current_millis;
		}
		// 3. Clear pending bit (MUST), write 1 to clear.
		EXTI->PR = (1U << 0);
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
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  /* System interrupt init*/
  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* SysTick_IRQn interrupt configuration */
  NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),15, 0));

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();



  /* USER CODE BEGIN SysInit */
  SysTick_Config(SystemCoreClock / 1000); // 1ms tick

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */
  RCC->AHB1ENR |= RCC_GPIOC_CLK_ENABLE; // Enabling Clock for GPIOC register
  RCC->AHB1ENR |= (1U << 0);

  GPIOC->MODER &= GPIOC_PC14_PC15_MODER_CLEAR; // Clear the  state of bit 28 && 30
  GPIOC->MODER |= GPIOC_PC14_PC15_OUTPUT_MODE; // Set the state of bit  28 && 30
  GPIOA->MODER &= ~(3U << 0);

  GPIOA->PUPDR &= ~(3U << 0);
  GPIOA->PUPDR |= (1U << 0);

  // SYSCFG -> Connect PA0 to EXTI line 0
  SYSCFG->EXTICR[0] &= ~(0xF << 0); // clear
  SYSCFG->EXTICR[0] |=  (0x0 << 0); // set PA0

  // EXTI -> Falling edge (button pushed : HIGH->LOW)
  EXTI->FTSR |= (1U << 0);

  // EXTI -> Unmask line 0 (Give permission to interrupt)
  EXTI->IMR |= (1U << 0);

  // NVIC -> Set priority and enable
  NVIC_SetPriority(EXTI0_IRQn, 1);
  NVIC_EnableIRQ(EXTI0_IRQn);

  /* USER CODE END 2 */

  /* Infinite loop */
  while (1)
    {
	  	  // EMPTY -> CPU FREE TO DO ANOTHER TASK
	  	  // ALL REACTION HAPPENS IN EXTI0_IRQHandler
    }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_0)
  {
  }
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  LL_RCC_HSI_SetCalibTrimming(16);
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {

  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI)
  {

  }
  LL_Init1msTick(16000000);
  LL_SetSystemCoreClock(16000000);
  LL_RCC_SetTIMPrescaler(LL_RCC_TIM_PRESCALER_TWICE);
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
