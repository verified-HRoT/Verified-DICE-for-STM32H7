/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include <stdio.h>

#include "common/locking.h"
#include "common/machine.h"
#include "fw/common/log_ram.h"
#include "fw/common/shared_mem_persistent.h"

static bool targetStderr = false;
static bool targetDebugWindow = true; // This is only used in Windows

void LoggerTargetStderr(bool targetStderr_) { targetStderr = targetStderr_; }

void LoggerTargetDebugWindow(bool targetDebugWindow_) {
  targetDebugWindow = targetDebugWindow_;
}

void TargetPrintString(const char *str) {
#ifdef _WIN32
  if (IsDebuggerPresent() && targetDebugWindow) {
    OutputDebugStringA(str);
  } else {
    fputs(str, targetStderr ? stderr : stdout);
  }
#else
  fputs(str, targetStderr ? stderr : stdout);
#endif

  LogToMemory(str);
  LogToPersistentMemory(str);
}
