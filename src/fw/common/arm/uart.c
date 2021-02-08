/*
 * Copyright (c) 2017 STMicroelectronics. All rights reserved.
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 */
/*
 * Copyright(c) 2020 Microsoft. All rights reserved.
 */

#include <stdio.h>

#include <stm32h7xx_hal.h>

// Definitions for USARTx clock resources
#define USARTx USART3
#define USARTx_CLK_ENABLE() __HAL_RCC_USART3_CLK_ENABLE()
#define DMAx_CLK_ENABLE() __HAL_RCC_DMA2_CLK_ENABLE()
#define USARTx_RX_GPIO_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()

#define USARTx_FORCE_RESET() __HAL_RCC_USART3_FORCE_RESET()
#define USARTx_RELEASE_RESET() __HAL_RCC_USART3_RELEASE_RESET()

// Definitions for USARTx pins
#define USARTx_TX_PIN GPIO_PIN_9
#define USARTx_TX_GPIO_PORT GPIOD
#define USARTx_TX_AF GPIO_AF7_USART3
#define USARTx_RX_PIN GPIO_PIN_8
#define USARTx_RX_GPIO_PORT GPIOD
#define USARTx_RX_AF GPIO_AF7_USART3

// Definitions for USARTx's DMA
#define USARTx_TX_DMA_STREAM DMA2_Stream7
#define USARTx_RX_DMA_STREAM DMA2_Stream1

// Definitions for USARTx's DMA request
#define USARTx_TX_DMA_REQUEST DMA_REQUEST_USART3_TX
#define USARTx_RX_DMA_REQUEST DMA_REQUEST_USART3_RX

// Definitions for USARTx's NVIC
#define USARTx_DMA_TX_IRQn DMA2_Stream7_IRQn
#define USARTx_DMA_RX_IRQn DMA2_Stream1_IRQn
#define USARTx_DMA_TX_IRQHandler DMA2_Stream7_IRQHandler
#define USARTx_DMA_RX_IRQHandler DMA2_Stream1_IRQHandler

// Definitions for USARTx's NVIC
#define USARTx_IRQn USART3_IRQn
#define USARTx_IRQHandler USART3_IRQHandler

static DMA_HandleTypeDef hdma_tx;
static DMA_HandleTypeDef hdma_rx;

static UART_HandleTypeDef huart;

// This function handles DMA interrupt requests
void USARTx_DMA_RX_IRQHandler(void) { HAL_DMA_IRQHandler(huart.hdmarx); }

// This function handles DMA interrupt requests
void USARTx_DMA_TX_IRQHandler(void) { HAL_DMA_IRQHandler(huart.hdmatx); }

// This function handles UART interrupt requests
void USARTx_IRQHandler(void) { HAL_UART_IRQHandler(&huart); }

// UART MSP initialization
//
// This function configures the hardware resources:
// - Peripheral's clock enable
// - Peripheral's GPIO Configuration
// - DMA configuration for transmission request by peripheral
// - NVIC configuration for DMA interrupt request enable
static void UART_MspInit(UART_HandleTypeDef *huart) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  // 1 - Enable peripherals and GPIO clocks

  // Enable GPIO TX/RX clock
  USARTx_TX_GPIO_CLK_ENABLE();
  USARTx_RX_GPIO_CLK_ENABLE();

  // Enable USARTx clock
  USARTx_CLK_ENABLE();

  // Enable DMA clock
  DMAx_CLK_ENABLE();

  // 2 - Configure peripheral GPIO

  // UART TX GPIO pin configuration
  GPIO_InitStruct.Pin = USARTx_TX_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = USARTx_TX_AF;

  HAL_GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStruct);

  // UART RX GPIO pin configuration
  GPIO_InitStruct.Pin = USARTx_RX_PIN;
  GPIO_InitStruct.Alternate = USARTx_RX_AF;

  HAL_GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStruct);

  // 3 - Configure the DMA

  // Configure the DMA handler for transmission process
  hdma_tx.Instance = USARTx_TX_DMA_STREAM;
  hdma_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
  hdma_tx.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_tx.Init.MemInc = DMA_MINC_ENABLE;
  hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  hdma_tx.Init.Mode = DMA_NORMAL;
  hdma_tx.Init.Priority = DMA_PRIORITY_LOW;
  hdma_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
  hdma_tx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
  hdma_tx.Init.MemBurst = DMA_MBURST_INC4;
  hdma_tx.Init.PeriphBurst = DMA_PBURST_INC4;
  hdma_tx.Init.Request = USARTx_TX_DMA_REQUEST;

  HAL_DMA_Init(&hdma_tx);

  // Associate the initialized DMA handle to the UART handle
  __HAL_LINKDMA(huart, hdmatx, hdma_tx);

  // Configure the DMA handler for reception process
  hdma_rx.Instance = USARTx_RX_DMA_STREAM;
  hdma_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
  hdma_rx.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_rx.Init.MemInc = DMA_MINC_ENABLE;
  hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  hdma_rx.Init.Mode = DMA_NORMAL;
  hdma_rx.Init.Priority = DMA_PRIORITY_HIGH;
  hdma_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
  hdma_rx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
  hdma_rx.Init.MemBurst = DMA_MBURST_INC4;
  hdma_rx.Init.PeriphBurst = DMA_PBURST_INC4;
  hdma_rx.Init.Request = USARTx_RX_DMA_REQUEST;

  HAL_DMA_Init(&hdma_rx);

  // Associate the initialized DMA handle to the the UART handle
  __HAL_LINKDMA(huart, hdmarx, hdma_rx);

  // 4 - Configure the NVIC for DMA

  // NVIC configuration for DMA transfer complete interrupt
  HAL_NVIC_SetPriority(USARTx_DMA_TX_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(USARTx_DMA_TX_IRQn);

  // NVIC configuration for DMA transfer complete interrupt
  HAL_NVIC_SetPriority(USARTx_DMA_RX_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USARTx_DMA_RX_IRQn);

  // NVIC for USART, to catch the TX complete
  HAL_NVIC_SetPriority(USARTx_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(USARTx_IRQn);
}

// UART MSP de-initialization
//
// This function frees the hardware resources:
// - Disable the Peripheral's clock
// - Revert GPIO, DMA and NVIC configuration to their default state
static void UART_MspDeInit(UART_HandleTypeDef *huart) {
  // 1 - Reset peripherals

  USARTx_FORCE_RESET();
  USARTx_RELEASE_RESET();

  // 2 - Disable peripherals and GPIO clocks

  // Configure USARTx Tx as alternate function
  HAL_GPIO_DeInit(USARTx_TX_GPIO_PORT, USARTx_TX_PIN);
  // Configure USARTx Rx as alternate function
  HAL_GPIO_DeInit(USARTx_RX_GPIO_PORT, USARTx_RX_PIN);

  // 3 - Disable the DMA

  // De-Initialize the DMA channel associated to reception process
  if (huart->hdmarx != 0) {
    HAL_DMA_DeInit(huart->hdmarx);
  }
  // De-Initialize the DMA channel associated to transmission process
  if (huart->hdmatx != 0) {
    HAL_DMA_DeInit(huart->hdmatx);
  }

  // 4 - Disable the NVIC for DMA

  HAL_NVIC_DisableIRQ(USARTx_DMA_TX_IRQn);
  HAL_NVIC_DisableIRQ(USARTx_DMA_RX_IRQn);
}

void UART_Init(void) {
  // Put the USART peripheral in asynchronous mode (UART Mode)

  // UART configured as follows:
  // - Word Length = 8 Bits
  // - Stop Bit = One Stop bit
  // - Parity = None
  // - BaudRate = 115200 baud
  // - Hardware flow control disabled (RTS and CTS signals)

  huart.Instance = USARTx;

  huart.Init.BaudRate = 115200;
  huart.Init.WordLength = UART_WORDLENGTH_8B;
  huart.Init.StopBits = UART_STOPBITS_1;
  huart.Init.Parity = UART_PARITY_NONE;
  huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart.Init.Mode = UART_MODE_RX | UART_MODE_TX;
  huart.Init.ClockPrescaler = UART_PRESCALER_DIV32;
  huart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_RXOVERRUNDISABLE_INIT;
  huart.AdvancedInit.OverrunDisable = UART_ADVFEATURE_OVERRUN_DISABLE;

  // Register callbacks before calling HAL_UART_Init()
  if (HAL_UART_RegisterCallback(&huart, HAL_UART_MSPINIT_CB_ID, UART_MspInit) !=
      HAL_OK) {
    // We can't report the failure in any way: there is no UART
    while (1)
      ;
  }

  if (HAL_UART_RegisterCallback(&huart, HAL_UART_MSPDEINIT_CB_ID,
                                UART_MspDeInit) != HAL_OK) {
    // We can't report the failure in any way: there is no UART
    while (1)
      ;
  }

  if (HAL_UART_DeInit(&huart) != HAL_OK) {
    // We can't report the failure in any way: there is no UART
    while (1)
      ;
  }

  if (HAL_UART_Init(&huart) != HAL_OK) {
    // We can't report the failure in any way: there is no UART
    while (1)
      ;
  }
}

// Retargets the C library printf function to the USART. Returns 'ch'.
int __io_putchar(int ch) {
  if (ch == '\n') {
    HAL_UART_Transmit(&huart, (uint8_t *)"\r\n", 2U, 0xFFFFU);
  } else {
    HAL_UART_Transmit(&huart, (uint8_t *)&ch, 1U, 0xFFFFU);
  }

  return ch;
}

HAL_StatusTypeDef UART_Send(uint8_t *data, uint16_t len, uint32_t timeout) {
  return HAL_UART_Transmit(&huart, data, len, timeout);
}

HAL_StatusTypeDef UART_Receive(uint8_t *data, uint16_t len, uint32_t timeout) {
  return HAL_UART_Receive(&huart, data, len, timeout);
}

void UART_Flush(void) { __HAL_UART_FLUSH_DRREGISTER(&huart); }
