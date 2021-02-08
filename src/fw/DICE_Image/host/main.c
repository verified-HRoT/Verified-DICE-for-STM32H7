/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include "common/logger.h"

#include "fw/DICE_Image/DICE_main.h"

int DICE_main_host(void) {
  int ret = DICE_main();
  if (ret == 0) {
    HW_LOG_DEBUG("Booting L0...");
    return 0;
  }

  HW_LOG_FAIL("Execution stopped");
  return -1;
}
