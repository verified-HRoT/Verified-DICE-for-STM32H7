/*
 * Copyright (c) 2017 STMicroelectronics. All rights reserved.
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 */
/*
 * Copyright (c) 2019-2020 Microsoft. All rights reserved.
 */

#if HW_ENABLE_WATCHDOG

#include <stm32h7xx_hal.h>

#include "common/logger.h"
#include "fw/common/system_reset.h"
#include "global/timeouts.h"

// Software watchdog
// -----------------

// Definitions for TIMx clock resources
#define TIMx TIM3
#define TIMx_CLK_ENABLE() __HAL_RCC_TIM3_CLK_ENABLE()
#define TIMx_CLK_DISABLE() __HAL_RCC_TIM3_CLK_DISABLE()

// Definitions for TIMx's NVIC
#define TIMx_IRQn TIM3_IRQn
#define TIMx_IRQHandler TIM3_IRQHandler

static TIM_HandleTypeDef TimHandle;

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim) {
  (void)htim;

  // TIMx Peripheral clock enable
  TIMx_CLK_ENABLE();

  // Configure the NVIC for TIMx. Set TIMx priority
  HAL_NVIC_SetPriority(TIMx_IRQn, 3, 0);

  // Enable the TIMx global interrupt
  HAL_NVIC_EnableIRQ(TIMx_IRQn);
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim) {
  (void)htim;

  // TIMx Peripheral clock enable
  TIMx_CLK_DISABLE();

  // Enable the TIMx global interrupt
  HAL_NVIC_DisableIRQ(TIMx_IRQn);
}

void TIMx_IRQHandler(void) { HAL_TIM_IRQHandler(&TimHandle); }

static TimeMsType last_refresh = 0;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  (void)htim;

  TimeMsType now = GetTimeNow();

  // In case of a weird error, go back to a known state and skip the watchdog
  // check once.
  if (now < last_refresh) {
    last_refresh = GetTimeNow();
    return;
  }

  TimeDeltaMsType delta = now - last_refresh;

  if (delta > TIMEOUT_SW_WATCHDOG_SECONDS * 1000) {
    HW_LOG_FAIL("Software watchdog reset");
    System_Reset(SOFTWARE_RESET);
  }
}

#define SW_PERIOD_VALUE ((4000 - 1) * TIMEOUT_SW_WATCHDOG_SECONDS)
static_assert(SW_PERIOD_VALUE <= 0xFFFF, "SW watchdog timeout too big");

// Returns 0 on success
int SW_Watchdog_Init(void) {
  uint32_t uwPrescalerValue = (uint32_t)(SystemCoreClock / (2 * 10000)) - 1;

  TimHandle.Instance = TIMx;
  TimHandle.Init.Period = SW_PERIOD_VALUE;
  TimHandle.Init.Prescaler = uwPrescalerValue;
  TimHandle.Init.ClockDivision = 0;
  TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
  TimHandle.Init.RepetitionCounter = 0;

  if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK) {
    return -1;
  }

  // Start the TIM Base generation in interrupt mode
  if (HAL_TIM_Base_Start_IT(&TimHandle) != HAL_OK) {
    return -1;
  }

  return 0;
}

void SW_Watchdog_Reload(void) { last_refresh = GetTimeNow(); }

int SW_Watchdog_End(void) {
  if (HAL_TIM_Base_Stop_IT(&TimHandle) != HAL_OK) {
    return -1;
  }

  if (HAL_TIM_Base_DeInit(&TimHandle) != HAL_OK) {
    return -1;
  }

  return 0;
}

// Hardware watchdog
// -----------------

static IWDG_HandleTypeDef IwdgHandle;

#define HW_RELOAD_VALUE ((TIMEOUT_HW_WATCHDOG_SECONDS * LSI_VALUE) / 64U)
static_assert(HW_RELOAD_VALUE <= 0x0FFF, "HW watchdog timeout too big");

// Returns 0 on success
int HW_Watchdog_Init(void) {
  IwdgHandle.Instance = IWDG1;
  IwdgHandle.Init.Prescaler = IWDG_PRESCALER_64;
  IwdgHandle.Init.Reload = HW_RELOAD_VALUE;
  IwdgHandle.Init.Window = IWDG_WINDOW_DISABLE;

  if (HAL_IWDG_Init(&IwdgHandle) == HAL_OK)
    return 0;

  return -1;
}

void HW_Watchdog_Reload(void) {
  // Ideally, this function should do: HAL_IWDG_Refresh(&IwdgHandle);
  // However, that is only possible in the image that initializes the
  // watchdog. Later images won't have an initialized IwdgHandle struct. In
  // the later images it is needed to access the IWDG registers manually.
  WRITE_REG(IWDG1->KR, IWDG_KEY_RELOAD);
}

#else // if HW_ENABLE_WATCHDOG == 0

int SW_Watchdog_Init(void) { return 0; }

void SW_Watchdog_Reload(void) {
  // Nothing to do
}

int SW_Watchdog_End(void) { return 0; }

int HW_Watchdog_Init(void) { return 0; }

void HW_Watchdog_Reload(void) {
  // Nothing to do
}

#endif // HW_ENABLE_WATCHDOG
