/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

#include "common/machine.h"

EXTERNAL_C_BEGIN

typedef enum { HARDWARE_RESET = 0, SOFTWARE_RESET = 1 } system_reset_type;

// This function should reboot the system. However, it may not be possible (like
// in the host emulation of the FW), so the caller should make sure that
// returning from it doesn't cause any unintended effects.
void System_Reset(system_reset_type resetType);

EXTERNAL_C_END
