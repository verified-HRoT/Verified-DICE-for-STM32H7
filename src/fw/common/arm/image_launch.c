/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include <stm32h7xx_hal.h>

#include "fw/common/arm/image_launch.h"

// The implementation of this is in image_launch_asm.s
//
// This function clears the DTCM ram and jumps to the address specified in
// trampoline, which is supposed to be a trampoline to the firmware image to be
// loaded.
//
// The value specified in r0 is passed to the trampoline in register r0.
_Noreturn void LaunchImageAsm(uint32_t r0, uint32_t trampoline);

_Noreturn void LaunchImage(uintptr_t base_addr, void (*trampoline)(uint32_t)) {
  // Disable all interrupts
  __disable_irq();
  RCC->CIER = 0x00000000;

  // Turn off caches gracefully
  SCB_DisableICache();
  SCB_DisableDCache();

  // Clear memory and launch image through the provided trampoline
  LaunchImageAsm(base_addr, (uint32_t)trampoline);

  // This point shouldn't be reached. At this point we can't trust our stack
  // pointer or anything so we can't do anything at all.
  while (1)
    ;
}
