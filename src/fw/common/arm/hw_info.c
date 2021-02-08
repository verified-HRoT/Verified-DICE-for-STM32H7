/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include <stdio.h>
#include <string.h>

//#include <stm32h7xx_ll_cortex.h>
#include <stm32h7xx_ll_utils.h>

static char hw_info_str[256];

const char *GetHwInfoString(void) {
  // Only fill the string the first time this function is called
  if (strlen(hw_info_str) > 0)
    return hw_info_str;

  // Information in the string:
  // - Unique Device Identifier (96-bit UID)
  snprintf(hw_info_str, sizeof(hw_info_str),
           "Hardware: Arm\n"
           "UID: [0x%08lX, 0x%08lX, 0x%08lX]\n"
           "Flash size: %lu KB\n"
           "Package type: %lu\n"
           "CPUID: 0x%08lX",
           LL_GetUID_Word0(), LL_GetUID_Word1(), LL_GetUID_Word2(),
           LL_GetFlashSize(), LL_GetPackageType(), SCB->CPUID);

  return hw_info_str;
}
