/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

#include <stdint.h>

// Jump to the image located at base_addr. The address must be the base address
// of the interrupt vectors.
_Noreturn void LaunchImage(uintptr_t base_addr, void (*trampoline)(uint32_t));

// Simple image launcher that jumps to the firmware image at the specified
// address. It shouldn't be used on its own, but through LaunchImage().
void LaunchImageTrampoline(uint32_t image_address);
