/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

#include "machine.h"

#ifdef _WIN32 // Windows

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h> // this MUST be the only place we include windows.h

typedef CRITICAL_SECTION LockType;

#define AQUIRES_LOCK(x) _Acquires_lock_(x)
#define RELEASES_LOCK(x) _Releases_lock_(x)

#elif BIG_BUILD // Linux

#include <pthread.h>

typedef pthread_mutex_t LockType;

#define AQUIRES_LOCK(x)
#define RELEASES_LOCK(x)

#else

typedef int LockType;

#define AQUIRES_LOCK(x)
#define RELEASES_LOCK(x)

#endif

EXTERNAL_C void InitLock(LockType *lock);

AQUIRES_LOCK(lock)
EXTERNAL_C void EnterLock(LockType *lock);

RELEASES_LOCK(lock)
EXTERNAL_C void LeaveLock(LockType *lock);
