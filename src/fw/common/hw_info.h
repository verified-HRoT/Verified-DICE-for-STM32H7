/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

#include "common/machine.h"

// This function returns a string that has some hardware information. This is
// already allocated and can't be modified. This is purely to be passed as debug
// information.
EXTERNAL_C const char *GetHwInfoString(void);
