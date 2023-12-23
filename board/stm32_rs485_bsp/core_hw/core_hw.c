/*
 * core_hw.c
 *
 *  Created on: Sep 16, 2020
 *      Author: quangnd
 */

#include "core_hw.h"
#include "core.h"


//IWDG_HandleTypeDef hiwdg;

static void system_clock_config(void);
static void system_tick_config(void);
static void iwdg_timer_config(void);

void core_hw_init(void){
	system_clock_config();
	system_tick_config();

#if ENABLE_IWDG_TIMER
	iwdg_timer_config();
#endif
}

static void system_clock_config(void){
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

	  /** Initializes the RCC Oscillators according to the specified parameters
	  * in the RCC_OscInitTypeDef structure.
	  */
	  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
	  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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

	  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
	  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
	  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	  {
	    Error_Handler();
	  }
}

static void system_tick_config(void){
	SysTick_Config(SystemCoreClock/SYSTICK_FREQ_Hz);
//	HAL_NVIC_SetPriority(SysTick_IRQn,4, 0);// 10ms
}

//static void iwdg_timer_config(void){
//	/* Indepedent Watchdog timer Reset CPU every 5s when error */
//	  hiwdg.Instance = IWDG;
//	  /* IWDG using LSI (40kHz), 256*781/40000 ~ 5s */
//	  hiwdg.Init.Prescaler = IWDG_PRESCALER_256;
//	  hiwdg.Init.Reload = 781;
//	  if (HAL_IWDG_Init(&hiwdg) != HAL_OK){
//		  Error_Handler();
//	  }
//	  HAL_IWDG_Init(&hiwdg);
//}
