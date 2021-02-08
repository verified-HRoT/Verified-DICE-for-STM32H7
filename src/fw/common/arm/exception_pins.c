/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include <stm32h7xx_hal.h>

#define PIN_COL0_TC0_SECEX_NUMBER GPIO_PIN_0
#define PIN_COL0_TC0_SECEX_PORT GPIOE
#define PIN_COL0_TC0_SECEX_CLK_ENABLE() __HAL_RCC_GPIOE_CLK_ENABLE()

#define PIN_COL1_TC0_SECEX_NUMBER GPIO_PIN_1
#define PIN_COL1_TC0_SECEX_PORT GPIOE
#define PIN_COL1_TC0_SECEX_CLK_ENABLE() __HAL_RCC_GPIOE_CLK_ENABLE()

int ExceptionPin_IsSet(int col) {
  GPIO_PinState state = GPIO_PIN_RESET;

  if (col == 0) {
    state =
        HAL_GPIO_ReadPin(PIN_COL0_TC0_SECEX_PORT, PIN_COL0_TC0_SECEX_NUMBER);
  } else if (col == 1) {
    state =
        HAL_GPIO_ReadPin(PIN_COL1_TC0_SECEX_PORT, PIN_COL1_TC0_SECEX_NUMBER);
  }

  if (state == GPIO_PIN_SET)
    return 1;

  return 0;
}

void ExceptionPins_Init(void) {
  // Settings for all pins
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

  // Enable GPIO clocks
  PIN_COL0_TC0_SECEX_CLK_ENABLE();
  PIN_COL1_TC0_SECEX_CLK_ENABLE();

  // Configure COL0_TC0_SECEX
  GPIO_InitStruct.Pin = PIN_COL0_TC0_SECEX_NUMBER;
  HAL_GPIO_Init(PIN_COL0_TC0_SECEX_PORT, &GPIO_InitStruct);

  // Configure the output pin
  GPIO_InitStruct.Pin = PIN_COL1_TC0_SECEX_NUMBER;
  HAL_GPIO_Init(PIN_COL1_TC0_SECEX_PORT, &GPIO_InitStruct);
}
