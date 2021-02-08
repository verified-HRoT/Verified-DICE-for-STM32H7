/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include "common/logger.h"
#include "core/l0_engine/l0_engine.h"
#include "fw/common/image_header.h"
#include "fw/common/shared_mem_volatile.h"
#include "fw/common/watchdog.h"

// This is a wrapper that calls L0Core with the right arguments
static void RunL0(void) {
  l0_params_t *l0_params = (l0_params_t *)GetShared_DICE_L0();
  l1_params_t *l1_params = (l1_params_t *)GetShared_L0_L1();

  // Clear shared memory with L1 before starting to use it
  CleanShared_L0_L1();

  // Call L0 engine to write shared information to share with L1
  image_header_t *header = (image_header_t *)GetL1ImageStart();

  uintptr_t l1_addr = (uintptr_t)GetL1ImageStart() + FIRMWARE_IMAGE_OFFSET;

  L0HandleArgs((uint8_t *)l1_addr, header->fw_size, l0_params, l1_params);

  // Clear shared memory with DICE before leaving L0
  CleanShared_DICE_L0();
}

int L0_main(void) {
  Watchdog_Reload();

  HW_LOG_INFO_STR("\n"
                  "+----------+\n"
                  "| L0 Image |\n"
                  "+----------+\n"
                  "\n"
                  "Copyright (c) 2020 Microsoft\n"
                  "Copyright (c) 2017-2019 STMicroelectronics\n"
                  "\n"
                  "Build time: " TIMESTAMP_STRING "\n"
                  "Commit: " GIT_COMMIT_DESCRIPTION "\n");

  Watchdog_Reload();

  // Run L0 core
  RunL0();

  // Return success
  return 0;
}
