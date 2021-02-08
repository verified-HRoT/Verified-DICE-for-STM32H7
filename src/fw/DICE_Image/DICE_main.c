/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include "common/logger.h"
#include "common/machine.h"
#include "core/dice_engine/dice_engine.h"
#include "crypto/crypto.h"
#include "fw/DICE_Image/reset.h"
#include "fw/common/image_header.h"
#include "fw/common/memmap.h"
#include "fw/common/option_bytes.h"
#include "fw/common/shared_mem_persistent.h"
#include "fw/common/shared_mem_volatile.h"
#include "fw/common/watchdog.h"

#include "protections.h"
#include "secret.h"

static int RunDICE(uint8_t *hw_measurement) {
  l0_params_t *l0_params = (l0_params_t *)GetShared_DICE_L0();

  // Clear shared memory region with L0 before starting to use it.
  CleanShared_DICE_L0();

  // Call DICE engine to write shared information to share with L0
  // -------------------------------------------------------------

  const int ret = DICEHandleArgsAndDice(
      hw_measurement, sizeof(l0_params->HWMeasurement), l0_params);

  return ret;
}

int DICE_main(void) {
  uint8_t hw_measurement[] = "***HW Measurement***"
                             "***HW Measurement***";

  HW_LOG_INFO_STR("\n"
                  "+------------+\n"
                  "| DICE Image |\n"
                  "+------------+\n"
                  "\n"
                  "Copyright (c) 2020 Microsoft\n"
                  "Copyright (c) 2017-2019 STMicroelectronics\n"
                  "\n"
                  "Build time: " TIMESTAMP_STRING "\n"
                  "Commit: " GIT_COMMIT_DESCRIPTION "\n");

  // If the hardware protections aren't set correctly stop right away
  if (CheckProtections() != 0) {
    HW_LOG_FAIL("CheckProtections() failed");
    return -1;
  }

  ResetReason_Update();

  const reset_reason reason = ResetReason_Get();
  HW_LOG_INFO("Reset reason: %s", ResetReason_GetString(reason));

  // TODO: This is done because the HW doesn't have a reset command yet.
  // Eventually we have to enter recovery mode if the reset reason is
  // different from a hardware reset. For now, the only way to reboot the HW
  // is doing a software reset.
  if (reason == RESET_REASON_WATCHDOG) {
    HW_LOG_FAIL("Watchdog reset. Entering recovery mode.");
    return -1;
  }

  // This code will only be executed during a normal boot (hardware reset)

  // Now that RDP protection is enabled we can generate the device secret.
  if (DeviceSecretInitialize() != 0) {
    HW_LOG_INFO("DeviceSecretInitialize() failed");
    return -1;
  }

  Watchdog_Reload();

  // Run DICE code
  return RunDICE(hw_measurement);
}
