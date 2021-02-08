/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include "locking.h"

#ifdef _WIN32

void InitLock(LockType *lock) { InitializeCriticalSection(lock); }

_Acquires_lock_(lock) void EnterLock(LockType *lock) {
  EnterCriticalSection(lock);
}

_Releases_lock_(lock) void LeaveLock(LockType *lock) {
  LeaveCriticalSection(lock);
}

#else

void InitLock(LockType *lock) {
#if BIG_BUILD
  pthread_mutex_init(lock, NULL);
#else
  lock = lock;
#endif
}

void EnterLock(LockType *lock) {
#if BIG_BUILD
  pthread_mutex_lock(lock);
#else
  lock = lock;
#endif
}

void LeaveLock(LockType *lock) {
#if BIG_BUILD
  pthread_mutex_unlock(lock);
#else
  lock = lock;
#endif
}

#endif
