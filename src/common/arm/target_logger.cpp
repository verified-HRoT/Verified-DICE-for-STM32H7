/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include <stdio.h>

#include "fw/common/log_ram.h"
#include "fw/common/shared_mem_persistent.h"

void TargetPrintString(const char *str) {
  fputs(str, stdout);

  LogToMemory(str);
  LogToPersistentMemory(str);
}
