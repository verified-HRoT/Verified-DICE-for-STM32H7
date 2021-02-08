/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include "common/logger.h"
#include "fw/L0_Image/l0_main.h"

int L0_main_host(void) {
  int ret = L0_main();
  if (ret == 0) {
    HW_LOG_DEBUG("Booting L1...");
    return 0;
  }

  HW_LOG_FAIL("Execution stopped");
  return -1;
}
