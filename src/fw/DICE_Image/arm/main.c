/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include <stm32h7xx_hal.h>
#include <stm32h7xx_ll_utils.h>
#include <stm32h7xx_nucleo_144.h>

#include "common/logger.h"
#include "fw/DICE_Image/DICE_main.h"
#include "fw/DICE_Image/arm/rss_definitions.h"
#include "fw/common/arm/backup_sram.h"
#include "fw/common/arm/hw_rng.h"
#include "fw/common/arm/image_launch.h"
#include "fw/common/arm/system_setup.h"
#include "fw/common/image_header.h"
#include "fw/common/memmap.h"
#include "fw/common/watchdog.h"

static _Noreturn void BootToL0(void) {
  uint32_t base_addr = (uintptr_t)GetL0ImageStart() + FIRMWARE_IMAGE_OFFSET;

  HW_LOG_INFO("Jumping to L0 at address 0x%08lX", base_addr);

  Backup_SRAM_Disable();

  // Launch through the RSS exit function. This works even if RSS isn't
  // enabled or if we are in a STM32H743 instead of STM32H753.
  LaunchImage(base_addr, RSS_TABLE->exitSecureArea);
}

// TODO: Remove this function!
static void CheckEmergencyRSSExit(void) {
  uint32_t package = LL_GetPackageType();
  const char *package_name = "";

  if (package == LL_UTILS_PACKAGETYPE_TQFP144) {
    package_name = "Nucleo board";
  } else if (package == LL_UTILS_PACKAGETYPE_TQFP176_UFBGA176) {
    package_name = "Vendor board";
  } else {
    package_name = "Unknown";
  }

  HW_LOG_DEBUG("Package type: %u (%s)", (unsigned int)package, package_name);

  // Only enable the emergency exit if the FW is running on a Nucleo board
  if (package == LL_UTILS_PACKAGETYPE_TQFP144) {
    HW_LOG_DEBUG("Checking RSS emergency exit");

    BSP_PB_Init(BUTTON_USER, BUTTON_MODE_GPIO);

    if (BSP_PB_GetState(BUTTON_USER) != 0) {
      HW_LOG_FAIL("Emergency RSS exit...");
      HAL_Delay(500);
    }
  }
}

int main(void) {
  System_Init();

  const TimeMsType start = GetTimeNow();
  TimeDeltaMsType elapsed = 0;

  Backup_SRAM_Enable();

  // TODO: Remove. This is here to give time to my terminal emulator to setup
  HAL_Delay(500);

  // TODO: This is used to exit RSS if there is a bad firmware. This should
  // only be used during development and removed once the firmware update
  // process is working
  CheckEmergencyRSSExit();

  if (HW_Watchdog_Init() != 0) {
    HW_LOG_FAIL("HW_Watchdog_Init() failed");
  }

  if (SW_Watchdog_Init() != 0) {
    HW_LOG_FAIL("SW_Watchdog_Init() failed");
  }

  HW_RNG_Init();

  int ret = DICE_main();

  HW_RNG_DeInit();

  SW_Watchdog_End();

  const TimeMsType now = GetTimeNow();
  elapsed = now - start;

  HW_LOG_DEBUG("DICE Boot time: %9ums", (unsigned int)elapsed);

  if (ret == 0) {
    HW_LOG_DEBUG("DICE succeeded. Booting L0...");
    BootToL0();
  }

  // This should never be reached. Wait for the watchdog to reboot the CPU.
  while (1)
    ;
}
