/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

#include <stm32h7xx_hal.h>

// Initialize UART peripheral
void UART_Init(void);

HAL_StatusTypeDef UART_Send(uint8_t *data, uint16_t len, uint32_t timeout);
HAL_StatusTypeDef UART_Receive(uint8_t *data, uint16_t len, uint32_t timeout);
void UART_Flush(void);
