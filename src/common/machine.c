/*
 * Copyright(c) 2020 Microsoft. All rights reserved.
 */

// determines the platform type, exposes some 'abstract' versions of key
// platform specific types. These are just ints really.

#include "machine.h"

// abstract types and wrapper functions specific to particular platform
// dependent services we implement here

#include "threading.h"
#include "timing.h"

#if defined(_WIN32)

#define WIN32_LEAN_AND_MEAN 1
#include <processthreadsapi.h>
#include <windows.h> // this MUST be the only place we include windows.h

ThreadIDType GetThreadID(void) { return (ThreadIDType)GetCurrentThreadId(); }

ProcessIDType GetProcessID(void) { return (ThreadIDType)GetCurrentProcessId(); }

void SetThreadName(ThreadHandleType id, const char *name) {
#ifdef _WIN32
  wchar_t buffer[100];
  int size = MultiByteToWideChar(CP_ACP, 0, name, -1, &buffer[0],
                                 sizeof(buffer) / sizeof(wchar_t));
  size = size;
  HRESULT hr = SetThreadDescription(id, &buffer[0]);
  hr = hr;
#endif
}

void platform_zeroize(void *ptr, size_t len) { SecureZeroMemory(ptr, len); }

#endif

#if defined(__linux__) // assuming x86/x64 linux

#include <pthread.h> // the only place we include pthread.h
#include <string.h>
#include <unistd.h>

// static_assert(sizeof(pthread_t) < sizeof(ThreadIDType);   // make sure it
// will fit.

ThreadIDType GetThreadID() { return (ThreadIDType)pthread_self(); }

ProcessIDType GetProcessID(void) { return (ThreadIDType)getpid(); }

void SetThreadName(ThreadHandleType id, const char *name) {
  // there is a pthreads way to do this TBD
  id = id;
  name = name;
}

void platform_zeroize(void *ptr, size_t len) { explicit_bzero(ptr, len); }

#endif

#if !defined(_WIN32) && !defined(__linux__) // must be raw hardware

#define BIG_BUILD 0

ThreadIDType GetThreadID() { return 0; }

ProcessIDType GetProcessID(void) { return 0; }

#include <string.h>

void platform_zeroize(void *ptr, size_t len) { explicit_bzero(ptr, len); }

#endif
