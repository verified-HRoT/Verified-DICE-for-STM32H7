/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include <memory.h>
#include <stdarg.h>
#include <stdio.h>

#include "locking.h"
#include "logger.h"
#include "machine.h"
#include "target_logger.h"
#include "threading.h"
#include "timing.h"

#if BIG_BUILD
#define MAX_LOG_LINE_LEN 284
#else
#define MAX_LOG_LINE_LEN 384
#endif

static enum LogLevel currentLevel = LL_DEBUG;

// used to adjust time we print so as to somewhat normalise what we see
static TimeMsType zeroTime = 0;

TimeMsType SetLogZeroTime(TimeMsType value) {
  const TimeMsType previous = zeroTime;
  zeroTime = value;
  return previous;
}

void ResetLogZeroTime(void) {
  HW_LOG_DEBUG("Reset zero time");
  SetLogZeroTime(GetTimeNow());
}

uint32_t GetLogTime(void) { return (uint32_t)(GetTimeNow() - zeroTime); }

// No formatting, no pre-pended info
void HWLogStr(enum LogLevel level, const char *str) {
  if (level < currentLevel)
    return;

  TargetPrintString(str);
  TargetPrintString("\n");
}

void HWLogChar(enum LogLevel level, char c) {
// disable for host side/testing for now.
// actually completely disable. Too noisy
#if 1 // BIG_BUILD
  level = level;
  c = c;
#else
  if (level < currentLevel)
    return;

#ifdef _WIN32
  char str[2] = {0, 0};
  str[0] = c;
  if (IsDebuggerPresent() && targetDebugWindow) {
    OutputDebugStringA(str);
  } else {
    fputc(c, targetStderr ? stderr : stdout);
  }
#else
  fputc(c, targetStderr ? stderr : stdout);
#endif
#endif
}

static const char *levelNames[] = {"trace", "secret", "debug",
                                   "info",  "fail",   "fatal"};

static int lockInited = 0;
static LockType loggerLock;

// better to call this in main than doing lazy init

void LoggerInit(void) {
  if (!lockInited) {
    InitLock(&loggerLock);
    lockInited = 1;
  }
}

void LoggerLevel(enum LogLevel level) { currentLevel = level; }

void LoggerLevelStr(const char *level) {
  for (size_t i = 0; i < sizeof(levelNames) / sizeof(*levelNames); i++) {
    if (strcmp(level, levelNames[i]) == 0) {
      currentLevel = (enum LogLevel)i;
      return;
    }
  }
}

void HWLogFmt(enum LogLevel level, const char *fileName, int lineNo,
              const char *functionName, const char *formatStr, ...) {
  char buffer[MAX_LOG_LINE_LEN] = {0};
  char *buf = &buffer[0];
  if (!lockInited)
    LoggerInit();

  if (level < currentLevel ||
      level >=
          LL_AFTER_LAST) // be very explicit otherwise warning 4770 pops up.
    return;

  static CONSTEXPR size_t maxFieldLen = 20;

  const size_t fNameLen = strlen(fileName);
  if (fNameLen > maxFieldLen)
    fileName = fileName + fNameLen - maxFieldLen; // reduce file name field

  const size_t funcLen = strlen(functionName);
  if (funcLen > maxFieldLen)
    functionName =
        functionName + funcLen - maxFieldLen; // reduce function name field

  EnterLock(&loggerLock);
#if BIG_BUILD
  const ThreadIDType threadID = GetThreadID();
  const int len =
      snprintf(buf, sizeof(buffer), "%9u [%6s] %20s:%-5d %8x %-20s  ",
               (unsigned int)GetLogTime(), levelNames[level], fileName, lineNo,
               (int)threadID, functionName);
#else
  (void)functionName;

  const int len = snprintf(buf, sizeof(buffer), "%6u [%6s] %18s:%-5d  ",
                           (unsigned int)(GetLogTime()), levelNames[level],
                           fileName, lineNo);
#endif
  LeaveLock(&loggerLock);

  if ((size_t)len < sizeof(buffer) - maxFieldLen) {
    const size_t remaining = sizeof(buffer) - len;
    char *dest = buf + len;

    va_list args;
    va_start(args, formatStr);
    EnterLock(&loggerLock);
    vsnprintf(dest, remaining - 1, formatStr, args);
    LeaveLock(&loggerLock);
    va_end(args);
  }
  HWLogStr(level, buf);
}

void HWLog(enum LogLevel level, const char *fileName, int lineNo,
           const char *functionName, const char *str) {
  char buffer[MAX_LOG_LINE_LEN] = {0};
  char *buf = &buffer[0]; // avoid warnings

  if (level < currentLevel ||
      level >=
          LL_AFTER_LAST) // be very explicit otherwise warning 4770 pops up.
    return;

  static CONSTEXPR size_t maxFieldLen = 20;
  const size_t fNameLen = strlen(fileName);
  if (fNameLen > maxFieldLen)
    fileName = fileName + fNameLen - maxFieldLen; // reduce file name field

  const size_t funcLen = strlen(functionName);
  if (funcLen > maxFieldLen)
    functionName =
        functionName + funcLen - maxFieldLen; // reduce function name field

  EnterLock(&loggerLock);
#if BIG_BUILD
  const ThreadIDType threadID = GetThreadID();

  snprintf(buf, sizeof(buffer), "%9u [%6s] %20s:%-5d %8x %-20s  ",
           (unsigned int)(GetLogTime()), levelNames[level], fileName, lineNo,
           (int)threadID, functionName);
#else
  (void)functionName;

  snprintf(buf, sizeof(buffer), "%6u [%6s] %18s:%-5d  ",
           (unsigned int)(GetLogTime()), levelNames[level], fileName, lineNo);

#endif
  LeaveLock(&loggerLock);

  TargetPrintString(&buf[0]);
  TargetPrintString(&str[0]);
  TargetPrintString("\n");
}

void HWLogStrHex(enum LogLevel level, const char *fileName, int lineNo,
                 const char *functionName, const char *str, const uint8_t *data,
                 size_t len, size_t splitAt) {
  char buffer[MAX_LOG_LINE_LEN] = {0};
  char *buf = &buffer[0]; // avoid warnings about pointer decay - probably more
                          // dangerous really
  char *ptr = buf;
  size_t soFar = 0;
  int count = 0;

  if (str != nullptr) {
    EnterLock(&loggerLock);
    count = snprintf(ptr, sizeof(buffer) - soFar, "%s", str);
    LeaveLock(&loggerLock);
    soFar += count;
    ptr += count;
  }

  const size_t maxBytes =
      (sizeof(buffer) - soFar) / 3 - 4; // allow ' 12' per byte

  if (len > maxBytes)
    len = maxBytes;
  for (size_t i = 0; i < len; i++) {
    EnterLock(&loggerLock);
    if ((splitAt > 0) && ((i + 1 % splitAt) == 0) && (i + 1 != len))
      count = snprintf(ptr, sizeof(buffer) - soFar, "%02x\n", data[i]);
    else
      count = snprintf(ptr, sizeof(buffer) - soFar, "%02x", data[i]);
    LeaveLock(&loggerLock);
    soFar += count;
    ptr += count;
  }

  HWLog(level, fileName, lineNo, functionName, buf);
}

void HWLogHex(enum LogLevel level, const char *fileName, int lineNo,
              const char *functionName, const uint8_t *data, size_t len) {
  HWLogStrHex(level, fileName, lineNo, functionName, nullptr, data, len, 0);
}

void HWLogHexV(enum LogLevel level, const char *fileName, int lineNo,
               const char *functionName, const void *data, size_t len) {
  const uint8_t *ptr = (const uint8_t *)(data);
  HWLogHex(level, fileName, lineNo, functionName, ptr, len);
}
