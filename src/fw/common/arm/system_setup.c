/*
 * Copyright (c) 2017 STMicroelectronics. All rights reserved.
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 */
/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include <stm32h7xx_hal.h>

#include "fw/common/arm/uart.h"

// This variable counts the number of milliseconds since boot
static volatile uint64_t time_since_boot = 0;

static void SystemClock_Config(void);

// This function is called from inside HAL_Init()
void HAL_MspInit(void) { __HAL_RCC_SYSCFG_CLK_ENABLE(); }

void System_Init(void) {
  // Enable the CPU Cache
  SCB_EnableICache();
  SCB_EnableDCache();

  // Release bits used for software reset. See System_Reset()
  __HAL_RCC_AHB3_RELEASE_RESET();

  // STM32H7xx HAL library initialization:
  //
  // - Systick timer is configured by default as source of time base, but
  //   user can eventually implement his proper time base source (a
  //   general purpose timer for example or other time source), keeping in
  //   mind that Time base duration should be kept 1ms since
  //   PPP_TIMEOUT_VALUEs are defined and handled in milliseconds basis.
  // - Set NVIC Group Priority to 4
  // - Low Level Initialization
  HAL_Init();

  // Configure the system clock to 400 MHz
  SystemClock_Config();

  // Enable interrupts in case they weren't enabled
  __enable_irq();

  // Enable UART
  UART_Init();
}

// This function handles SysTick Handler.
void SysTick_Handler(void) {
  time_since_boot++;

  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
}

uint64_t System_Time(void) {
  uint64_t t1, t2;

  // We could get interrupted while reading the 2 32-bit values. Make sure
  // that we get two valid readings in a row.
  while (1) {
    t1 = time_since_boot;
    t2 = time_since_boot;

    if (t1 == t2)
      break;
  }

  return t1;
}

// System Clock Configuration
//
// The system Clock is configured as follows:
//
//    System Clock source = PLL (HSE BYPASS)
//    SYSCLK(Hz)          = 400000000 (CPU Clock)
//    HCLK(Hz)            = 200000000 (AXI and AHBs Clock)
//    D1 APB3 Prescaler   = 2 (APB3 Clock  100MHz)
//    D2 APB1 Prescaler   = 2 (APB1 Clock  100MHz)
//    D2 APB2 Prescaler   = 2 (APB2 Clock  100MHz)
//    D3 APB4 Prescaler   = 2 (APB4 Clock  100MHz)
//    HSI Frequency(Hz)   = 64000000
//    VDD(V)              = 3.3
//    Flash Latency(WS)   = 2
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  HAL_StatusTypeDef ret = HAL_OK;

  // Supply configuration update enable
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  // Configure the main internal regulator output voltage
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {
  }

  // Initializes the CPU, AHB and APB busses clocks
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 50;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 8;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if (ret != HAL_OK) {
    // The UART hasn't been setup yet, there is nothing we can do
    while (1)
      ;
  }

  // Initializes the CPU, AHB and APB busses clocks
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 |
                                RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;
  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);
  if (ret != HAL_OK) {
    // The UART hasn't been setup yet, there is nothing we can do
    while (1)
      ;
  }

  PeriphClkInitStruct.PeriphClockSelection =
      RCC_PERIPHCLK_USART3 | RCC_PERIPHCLK_SPI1 | RCC_PERIPHCLK_I2C3;
  PeriphClkInitStruct.Spi123ClockSelection = RCC_SPI123CLKSOURCE_PLL;
  PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_HSI;
  PeriphClkInitStruct.I2c123ClockSelection = RCC_I2C123CLKSOURCE_D2PCLK1;
  ret = HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
  if (ret != HAL_OK) {
    // The UART hasn't been setup yet, there is nothing we can do
    while (1)
      ;
  }

  // Note: The activation of the I/O Compensation Cell is recommended with
  // communication interfaces (GPIO, SPI, FMC, QSPI ...) when operating at high
  // frequencies(please refer to product datasheet). The I/O Compensation Cell
  // activation procedure requires:
  //
  // - The activation of the CSI clock
  // - The activation of the SYSCFG clock
  // - Enabling the I/O Compensation Cell: set bit[0] of register SYSCFG_CCCSR
  //
  // To do this please uncomment the following code:
#if 0
    __HAL_RCC_CSI_ENABLE() ;
    __HAL_RCC_SYSCFG_CLK_ENABLE() ;
    HAL_EnableCompensationCell();
#endif
}
