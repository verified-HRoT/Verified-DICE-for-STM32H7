/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

#include <stddef.h>

#include "common/machine.h"

EXTERNAL_C_BEGIN

// Initializes hardware RNG. Returns 0 on success
int HW_RNG_Init(void);

// Deinitializes hardware RNG. Returns 0 on success
int HW_RNG_DeInit(void);

// Returns 0 on success. Fills a buffer with random numbers
int HW_RNG_Rand(unsigned char *buffer, size_t size);

EXTERNAL_C_END
