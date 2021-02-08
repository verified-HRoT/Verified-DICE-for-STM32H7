/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

#include "common/machine.h"

#include <stdint.h>

// This function performs general hardware initialization (clocks, etc)
EXTERNAL_C void System_Init(void);

// Time in milliseconds since system boot
EXTERNAL_C uint64_t System_Time(void);
