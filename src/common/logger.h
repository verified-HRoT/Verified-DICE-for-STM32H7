/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

#include <stdint.h>

#include "machine.h"
#include "timing.h"

EXTERNAL_C_BEGIN

// The following definitions are needed so that we can change the value of
// HW_MIN_LOG_LEVEL from the build system. They shouldn't be used in the code.
#define LOG_LEVEL_TRACE 0
#define LOG_LEVEL_SECRET 1
#define LOG_LEVEL_DEBUG 2
#define LOG_LEVEL_INFO 3
#define LOG_LEVEL_FAIL 4
#define LOG_LEVEL_FATAL 5

// HW_MIN_LOG_LEVEL defines the log messages that are compiled in the fw
// binary. Only the messages of the specified level or above are compiled in.
// Instead of defaulting to some value, error if it hasn't been defined. This is
// done so that all the files are built to a known value rather than some to the
// specified one and some to the default one. The build system should define it
// for all files.
#ifndef HW_MIN_LOG_LEVEL
#error "HW_MIN_LOG_LEVEL not defined!"
#endif

enum LogLevel {
  LL_FIRST = 0,
  LL_TRACE = LOG_LEVEL_TRACE,
  LL_SECRET = LOG_LEVEL_SECRET,
  LL_DEBUG = LOG_LEVEL_DEBUG, // Events useful for debugging
  LL_INFO = LOG_LEVEL_INFO,   // Important events that should be logged even in
                              // release mode
  LL_FAIL = LOG_LEVEL_FAIL,   // Important failures that should always be logged
  LL_FATAL =
      LOG_LEVEL_FATAL, // Fatal errors that lead to a termination of the program
  LL_AFTER_LAST
};

void LoggerInit(int useStderr);
void LoggerLevel(enum LogLevel level);
void LoggerLevelStr(const char *level);

// various means to reset the log output times
// to make reading the logs easier.
TimeMsType SetLogZeroTime(TimeMsType value);
void ResetLogZeroTime(void);
uint32_t GetLogTime(void);

void HWLogFmt(enum LogLevel level, const char *fileName, int lineNo,
              const char *functionName, const char *formatStr, ...);

void HWLog(enum LogLevel level, const char *fileName, int lineNo,
           const char *functionName, const char *str);

void HWLogStr(enum LogLevel level, const char *str);
void HWLogChar(enum LogLevel level, char c);

// allows for taking a descriptive string and spliting the hex dump
void HWLogStrHex(enum LogLevel level, const char *fileName, int lineNo,
                 const char *functionName, const char *str, const uint8_t *data,
                 size_t len, size_t splitAt);

void HWLogHex(enum LogLevel level, const char *fileName, int lineNo,
              const char *functionName, const uint8_t *data, size_t len);
void HWLogHexV(enum LogLevel level, const char *fileName, int lineNo,
               const char *functionName, const void *data, size_t len);

#if HW_MIN_LOG_LEVEL <= LOG_LEVEL_TRACE
#define HW_LOG_TRACE(...)                                                      \
  HWLogFmt(LL_TRACE, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define HW_LOG_TRACE_HEX(ptr, len)                                             \
  HWLogHex(LL_TRACE, __FILE__, __LINE__, __FUNCTION__, ptr, len)
#define HW_LOG_TRACE_STR(str) HWLogStr(LL_TRACE, str)
#define HW_TRACE_CHAR(c) HWLogChar(LL_TRACE, c)
#else
#define HW_LOG_TRACE(...)                                                      \
  if (0) {                                                                     \
    HWLogFmt(LL_TRACE, "", 0, "", ##__VA_ARGS__);                              \
  }
#define HW_LOG_TRACE_HEX(ptr, len)                                             \
  {                                                                            \
    (void)(ptr);                                                               \
    (void)(len);                                                               \
  }
#define HW_LOG_TRACE_STR(str) (void)(str)
#define HW_TRACE_CHAR(c) (void)(c)
#endif

#if HW_MIN_LOG_LEVEL <= LOG_LEVEL_SECRET
#define HW_LOG_SECRET(...)                                                     \
  HWLogFmt(LL_SECRET, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define HW_LOG_SECRET_HEX(ptr, len)                                            \
  HWLogHex(LL_SECRET, __FILE__, __LINE__, __FUNCTION__, ptr, len)
#define HW_LOG_SECRET_STR(str) HWLogStr(LL_SECRET, str)
#define HW_SECRET_CHAR(c) HWLogChar(LL_SECRET, c)
#else
#define HW_LOG_SECRET(...)                                                     \
  if (0) {                                                                     \
    HWLogFmt(LL_SECRET, "", 0, "", ##__VA_ARGS__);                             \
  }
#define HW_LOG_SECRET_HEX(ptr, len)                                            \
  {                                                                            \
    (void)(ptr);                                                               \
    (void)(len);                                                               \
  }
#define HW_LOG_SECRET_STR(str) (void)(str)
#define HW_SECRET_CHAR(c) (void)(c)
#endif

#if HW_MIN_LOG_LEVEL <= LOG_LEVEL_DEBUG
#define HW_LOG_DEBUG(...)                                                      \
  HWLogFmt(LL_DEBUG, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define HW_LOG_DEBUG_HEXV(ptr, len)                                            \
  HWLogHexV(LL_DEBUG, __FILE__, __LINE__, __FUNCTION__, ptr, len)
#define HW_LOG_DEBUG_HEX(ptr, len)                                             \
  HWLogHex(LL_DEBUG, __FILE__, __LINE__, __FUNCTION__, ptr, len)
#define HW_LOG_DEBUG_STR(str) HWLogStr(LL_DEBUG, str)
#define HW_DEBUG_CHAR(c) HWLogChar(LL_DEBUG, c)
#else
#define HW_LOG_DEBUG(...)                                                      \
  if (0) {                                                                     \
    HWLogFmt(LL_DEBUG, "", 0, "", ##__VA_ARGS__);                              \
  }
#define HW_LOG_DEBUG_HEXV(ptr, len)                                            \
  {                                                                            \
    (void)(ptr);                                                               \
    (void)(len);                                                               \
  }
#define HW_LOG_DEBUG_HEX(ptr, len)                                             \
  {                                                                            \
    (void)(ptr);                                                               \
    (void)(len);                                                               \
  }
#define HW_LOG_DEBUG_STR(str) (void)(str)
#define HW_DEBUG_CHAR(c) (void)(c)
#endif

#if HW_MIN_LOG_LEVEL <= LOG_LEVEL_INFO
#define HW_LOG_INFO(...)                                                       \
  HWLogFmt(LL_INFO, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define HW_LOG_INFO_HEX(ptr, len)                                              \
  HWLogHex(LL_INFO, __FILE__, __LINE__, __FUNCTION__, ptr, len)
#define HW_LOG_INFO_STR(str) HWLogStr(LL_INFO, str)
#define HW_INFO_CHAR(c) HWLogChar(LL_INFO, c)
#else
#define HW_LOG_INFO(...)                                                       \
  if (0) {                                                                     \
    HWLogFmt(LL_INFO, "", 0, "", ##__VA_ARGS__);                               \
  }
#define HW_LOG_INFO_HEX(ptr, len)                                              \
  {                                                                            \
    (void)(ptr);                                                               \
    (void)(len);                                                               \
  }
#define HW_LOG_INFO_STR(str) (void)(str)
#define HW_INFO_CHAR(c) (void)(c)
#endif

#if HW_MIN_LOG_LEVEL <= LOG_LEVEL_FAIL
#define HW_LOG_FAIL(...)                                                       \
  HWLogFmt(LL_FAIL, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define HW_LOG_FAIL_HEX(ptr, len)                                              \
  HWLogHex(LL_FAIL, __FILE__, __LINE__, __FUNCTION__, ptr, len)
#define HW_LOG_FAIL_STR(str) HWLogStr(LL_FAIL, str)
#define HW_FAIL_CHAR(c) HWLogChar(LL_FAIL, c)
#else
#define HW_LOG_FAIL(...)                                                       \
  if (0) {                                                                     \
    HWLogFmt(LL_FAIL, "", 0, "", ##__VA_ARGS__);                               \
  }
#define HW_LOG_FAIL_HEX(ptr, len)                                              \
  {                                                                            \
    (void)(ptr);                                                               \
    (void)(len);                                                               \
  }
#define HW_LOG_FAIL_STR(str) (void)(str)
#define HW_FAIL_CHAR(c) (void)(c)
#endif

#if BIG_BUILD

#define LOG_DEBUG_FMT(...)                                                     \
  HWLogFmt(LL_DEBUG, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define LOG_TRACE_FMT(...)                                                     \
  HWLogFmt(LL_TRACE, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define LOG_INFO_FMT(...)                                                      \
  HWLogFmt(LL_INFO, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define LOG_FAIL_FMT(...)                                                      \
  HWLogFmt(LL_FAIL, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

// the buffer size used constructing the output string is not enormous, these
// macros will allow for any size string by not attempting to use formatting.
// The call site details and the payload string are just put out one after the
// other.

#define LOG_DEBUG(str) HWLog(LL_DEBUG, __FILE__, __LINE__, __FUNCTION__, str)
#define LOG_TRACE(str) HWLog(LL_TRACE, __FILE__, __LINE__, __FUNCTION__, str)
#define LOG_INFO(str) HWLog(LL_INFO, __FILE__, __LINE__, __FUNCTION__, str)
#define LOG_FAIL(str) HWLog(LL_FAIL, __FILE__, __LINE__, __FUNCTION__, str)

#define LOG_DEBUG_CHAR(c) HWLogChar(LL_DEBUG, c)
#define LOG_TRACE_CHAR(c) HWLogChar(LL_TRACE, c)
#define LOG_INFO_CHAR(c) HWLogChar(LL_INFO, c)
#define LOG_FAIL_CHAR(c) HWLogChar(LL_FAIL, c)

#define LOG_DEBUG_STR(str) HWLogStr(LL_DEBUG, str)
#define LOG_TRACE_STR(str) HWLogStr(LL_TRACE, str)
#define LOG_INFO_STR(str) HWLogStr(LL_INFO, str)
#define LOG_FAIL_STR(str) HWLogStr(LL_FAIL, str)

#define LOG_DEBUG_HEX(ptr, len)                                                \
  HWLogHex(LL_DEBUG, __FILE__, __LINE__, __FUNCTION__, ptr, len)
#define LOG_TRACE_HEX(ptr, len)                                                \
  HWLogHex(LL_TRACE, __FILE__, __LINE__, __FUNCTION__, ptr, len)
#define LOG_INFO_HEX(ptr, len)                                                 \
  HWLogHex(LL_INFO, __FILE__, __LINE__, __FUNCTION__, ptr, len)
#define LOG_FAIL_HEX(ptr, len)                                                 \
  HWLogHex(LL_FAIL, __FILE__, __LINE__, __FUNCTION__, ptr, len)

#define LOG_DEBUG_STR_HEX(str, ptr, len)                                       \
  HWLogStrHex(LL_DEBUG, __FILE__, __LINE__, __FUNCTION__, str, ptr, len, 16)
#define LOG_TRACE_STR_HEX(str, ptr, len)                                       \
  HWLogStrHex(LL_TRACE, __FILE__, __LINE__, __FUNCTION__, str, ptr, len, 16)
#define LOG_INFO_STR_HEX(str, ptr, len)                                        \
  HWLogStrHex(LL_INFO, __FILE__, __LINE__, __FUNCTION__, str, ptr, len, 16)
#define LOG_FAIL_STR_HEX(str, ptr, len)                                        \
  HWLogStrHex(LL_FAIL, __FILE__, __LINE__, __FUNCTION__, str, ptr, len, 16)

#else // cannot use these macros on code which might run on the HW, the debug vs
      // non debug settings may be different

#define LOG_DEBUG_FMT #error
#define LOG_TRACE_FMT #error
#define LOG_INFO_FMT #error
#define LOG_FAIL_FMT #error

#endif

EXTERNAL_C_END
