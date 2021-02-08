/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include "common/logger.h"
#include "fw/common/arm/backup_sram.h"
#include "fw/common/arm/hw_rng.h"
#include "fw/common/arm/image_launch.h"
#include "fw/common/arm/system_setup.h"
#include "fw/common/image_header.h"
#include "fw/common/memmap.h"
#include "fw/common/watchdog.h"

#include "../l0_main.h"

static _Noreturn void BootToL1(void) {
  // Get L1 base address from L0 header
  const image_header_t *l0_header = GetL0ImageStart();
  uint32_t base_addr_1 = (uintptr_t)l0_header + l0_header->next_image_offset +
                         FIRMWARE_IMAGE_OFFSET;

  // Get L1 base address from linker information
  uint32_t base_addr_2 = (uintptr_t)GetL1ImageStart() + FIRMWARE_IMAGE_OFFSET;

  if (base_addr_1 != base_addr_2) {
    // This can happen if the header of L0 has the wrong offset for L1
    // because, for example, it hasn't been updated after changing the
    // address of L1 in the linkerscript.
    HW_LOG_FAIL("Base L1 address mismatch (0x%08lX, 0x%08lX)", base_addr_1,
                base_addr_2);
  }

  HW_LOG_INFO("Jumping to L1 at address 0x%08lX", base_addr_1);

  Backup_SRAM_Disable();

  // Launch through a simple trampoline
  LaunchImage(base_addr_1, LaunchImageTrampoline);
}

int main(void) {
  System_Init();

  const TimeMsType start = GetTimeNow();
  TimeDeltaMsType elapsed = 0;

  Backup_SRAM_Enable();

  if (SW_Watchdog_Init() != 0) {
    HW_LOG_FAIL("SW_Watchdog_Init() failed");
  }

  HW_RNG_Init();

  int ret = L0_main();

  HW_RNG_DeInit();

  SW_Watchdog_End();

  const TimeMsType now = GetTimeNow();
  elapsed = now - start;

  HW_LOG_INFO("L0 Boot time: %9ums", (unsigned int)elapsed);

  if (ret == 0) {
    HW_LOG_DEBUG("L0 succeeded. Booting L1...");
    BootToL1();
  }

  // This should never be reached. Wait for the watchdog to reboot the CPU.
  while (1)
    ;
}
