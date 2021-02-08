/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

#include "machine.h"

EXTERNAL_C ThreadIDType GetThreadID(void);
EXTERNAL_C void SetThreadName(ThreadHandleType id, const char *name);

typedef long ProcessIDType;
EXTERNAL_C ProcessIDType GetProcessID(void);
