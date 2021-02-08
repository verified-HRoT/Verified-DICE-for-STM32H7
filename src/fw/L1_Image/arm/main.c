/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include "common/logger.h"
#include "fw/common/arm/backup_sram.h"
#include "fw/common/arm/hw_rng.h"
#include "fw/common/arm/system_setup.h"
#include "fw/common/watchdog.h"

#include "../l1_main.h"

int main(void) {
  System_Init();

  const TimeMsType start = GetTimeNow();
  TimeDeltaMsType elapsed = 0;

  Backup_SRAM_Enable();

  if (SW_Watchdog_Init() != 0) {
    HW_LOG_FAIL("SW_Watchdog_Init() failed");
  }

  HW_RNG_Init();

  L1_main();

  HW_RNG_DeInit();

  SW_Watchdog_End();

  const TimeMsType now = GetTimeNow();
  elapsed = now - start;

  HW_LOG_DEBUG("L1 Boot time: %9ums", (unsigned int)elapsed);

  // If L1_main() returns, there has been a critical failure.

  // This should never be reached. Wait for the watchdog to reboot the CPU.
  while (1)
    ;
}
