/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

#include <stddef.h> // for size_t

/*

    A place for machine/OS specific wrappers.

    For example getting the current thread id.

    Interfaces are common to all platforms to avoid including
    windows.h or pthreads.h all over the codebase, slowing things down
    and making static analysis more fragile.

*/

// to avoid wanings in C code being compiled as c++ but which might be compiled
// as C and to reduce typing and ugliness for exposing C code to C++ code

#ifdef __cplusplus
#define CONSTEXPR constexpr
#define EXTERNAL_C extern "C"
#define EXTERNAL_C_BEGIN extern "C" {
#define EXTERNAL_C_END }
#else
#define CONSTEXPR const
#define EXTERNAL_C
#define EXTERNAL_C_BEGIN
#define EXTERNAL_C_END
#endif

#if defined(_MSC_VER)
#define ALIGNED(x) __declspec(align(x))
#elif defined(__GNUC__)
#define ALIGNED(x) __attribute__((aligned(x)))
#else
#error "Unknown compiler"
#endif

typedef long ThreadIDType;
typedef void *ThreadHandleType;

#if defined(                                                                   \
    _WIN32) // ie building for a windows x86 or x64 machine
            // https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=vs-2019

// set for windows and linux but zero for embedded H7
// use to switch off expensive things such as all the streams based logging

// ONLY USE THIS SWITCH to control building of platform conditionals.
// DO NOT use the explicit platform except in the machine abstraction
// code of machine.*

#define BIG_BUILD 1

#elif defined(__linux__) // assuming x86/x64 linux

#define BIG_BUILD 1

#else // must be raw hardware

#define BIG_BUILD 0

#endif // not linux nor windows

EXTERNAL_C void platform_zeroize(void *ptr, size_t len);
