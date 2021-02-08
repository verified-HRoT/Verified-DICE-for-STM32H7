/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

#include "common/machine.h"

EXTERNAL_C_BEGIN

// Backup SRAM can't be accessed before it is enabled. The following functions
// can be used to enable it or disable it, and thus allow access or not.
void Backup_SRAM_Enable(void);
void Backup_SRAM_Disable(void);

EXTERNAL_C_END
