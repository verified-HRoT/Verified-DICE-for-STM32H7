/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include <stm32h7xx_hal.h>

#define PIN_IN_NUMBER GPIO_PIN_7
#define PIN_IN_PORT GPIOG
#define PIN_IN_CLK_ENABLE() __HAL_RCC_GPIOG_CLK_ENABLE()

#define PIN_OUT_NUMBER GPIO_PIN_11
#define PIN_OUT_PORT GPIOE
#define PIN_OUT_CLK_ENABLE() __HAL_RCC_GPIOE_CLK_ENABLE()

static volatile int readyflag_out_set = 0;

void ReadyFlag_Out_Set(void) {
  HAL_GPIO_WritePin(PIN_OUT_PORT, PIN_OUT_NUMBER, GPIO_PIN_SET);

  readyflag_out_set = 1;
}

void ReadyFlag_Out_Reset(void) {
  HAL_GPIO_WritePin(PIN_OUT_PORT, PIN_OUT_NUMBER, GPIO_PIN_RESET);

  readyflag_out_set = 0;
}

int ReadyFlag_Out_IsSet(void) { return readyflag_out_set; }

int ReadyFlag_In_IsSet(void) {
  return HAL_GPIO_ReadPin(PIN_IN_PORT, PIN_IN_NUMBER);
}

void ReadyFlag_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  // Enable GPIO clocks
  PIN_IN_CLK_ENABLE();
  PIN_OUT_CLK_ENABLE();

  // Configure the input pin
  GPIO_InitStruct.Pin = PIN_IN_NUMBER;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

  HAL_GPIO_Init(PIN_IN_PORT, &GPIO_InitStruct);

  // Configure the output pin
  GPIO_InitStruct.Pin = PIN_OUT_NUMBER;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

  HAL_GPIO_Init(PIN_OUT_PORT, &GPIO_InitStruct);

  ReadyFlag_Out_Reset();
}
