/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include <stm32h7xx_hal.h>

RNG_HandleTypeDef RngHandle;

// Enable the RNG's clock
static void RNG_MspInit(RNG_HandleTypeDef *hrng) {
  (void)hrng;

  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

  // Select PLL output as RNG clock source
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RNG;
  PeriphClkInitStruct.RngClockSelection = RCC_RNGCLKSOURCE_PLL;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

  // RNG Peripheral clock enable
  __HAL_RCC_RNG_CLK_ENABLE();
}

// Disable the RNG's clock
static void RNG_MspDeInit(RNG_HandleTypeDef *hrng) {
  (void)hrng;

  // Enable RNG reset state
  __HAL_RCC_RNG_FORCE_RESET();

  // Release RNG from reset state
  __HAL_RCC_RNG_RELEASE_RESET();
}

int HW_RNG_Init(void) {
  RngHandle.Instance = RNG;

  HAL_RNG_RegisterCallback(&RngHandle, HAL_RNG_MSPINIT_CB_ID, RNG_MspInit);
  HAL_RNG_RegisterCallback(&RngHandle, HAL_RNG_MSPDEINIT_CB_ID, RNG_MspDeInit);

  if (HAL_RNG_DeInit(&RngHandle) != HAL_OK)
    return -1;

  if (HAL_RNG_Init(&RngHandle) != HAL_OK)
    return -2;

  return 0;
}

int HW_RNG_DeInit(void) {
  if (HAL_RNG_DeInit(&RngHandle) != HAL_OK)
    return -1;

  return 0;
}

int HW_RNG_Rand(unsigned char *buffer, size_t size) {
  while (size > 4) {
    uint32_t num;
    if (HAL_RNG_GenerateRandomNumber(&RngHandle, &num) != HAL_OK)
      return -1;

    *buffer++ = num & 0xFF;
    *buffer++ = (num >> 8) & 0xFF;
    *buffer++ = (num >> 16) & 0xFF;
    *buffer++ = (num >> 24) & 0xFF;

    size -= 4;
  }

  if (size > 0) {
    uint32_t num;
    if (HAL_RNG_GenerateRandomNumber(&RngHandle, &num) != HAL_OK)
      return -1;

    if (size-- > 0)
      *buffer++ = num & 0xFF;
    if (size-- > 0)
      *buffer++ = (num >> 8) & 0xFF;
    if (size-- > 0)
      *buffer++ = (num >> 16) & 0xFF;
    if (size-- > 0)
      *buffer++ = (num >> 24) & 0xFF;
  }

  return 0;
}
