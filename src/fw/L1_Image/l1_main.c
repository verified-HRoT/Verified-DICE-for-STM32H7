/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include <stdlib.h>

#include "common/logger.h"
#include "core/l1_params.h"
#include "fw/common/shared_mem_volatile.h"
#include "fw/common/watchdog.h"

#include "menu.h"
#include "tests.h"

int L1_main() {
  int test_failed = 0;

  Watchdog_Reload();

  HW_LOG_INFO_STR("\n"
                  "+----------+\n"
                  "| L1 Image |\n"
                  "+----------+\n"
                  "\n"
                  "Copyright (c) 2020 Microsoft\n"
                  "Copyright (c) 2017-2019 STMicroelectronics\n"
                  "\n"
                  "Build time: " TIMESTAMP_STRING "\n"
                  "Commit: " GIT_COMMIT_DESCRIPTION "\n");

  HW_LOG_SECRET("HW Measurement");
  const l1_params_t *l1_params = (const l1_params_t *)GetShared_L0_L1();
  HW_LOG_SECRET_HEX(l1_params->HWMeasurement, sizeof(l1_params->HWMeasurement));

  // TODO: Disable the tests?

  // Run tests
  test_failed |= Test_ReadDICEImage();
  test_failed |= Test_ReadKeyManifest();
  test_failed |= Test_ReadDICEL0SharedMemory();

  // If any of the tests fail, warn the user
  if (test_failed) {
    HW_LOG_FAIL("Some of the automated tests have failed!");
    HW_LOG_FAIL("");
  }

  L1_Menu();

  int ret = test_failed ? -1 : 0;

  return ret;
}
