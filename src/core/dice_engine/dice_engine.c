/*
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root.
 */

#include <assert.h>
#include <stdint.h>
#include <string.h>

// Derived from: https://github.com/Microsoft/RIoT

#include "common/logger.h"
#include "common/machine.h"
#include "core/l0_params.h"
#include "crypto/crypto.h"
#include "global/version.h"

#include "DICE_Engine_Core.h"

int DiceCore() {
  HW_LOG_TRACE("DICE: Begin");

  dice_return_code ret = dice_main();
  HW_LOG_DEBUG("DICE: Return Code %u", ret);

  HW_LOG_TRACE("DICE: End");
  return ret;
}

int DICEHandleArgsAndDice(const uint8_t *hw_measurement,
                          size_t hw_measurement_size, l0_params_t *l0_params) {
  // Copy HW measurement
  memcpy(l0_params->HWMeasurement, hw_measurement, hw_measurement_size);

  // Pass the version of DICE to L0
  l0_params->VersionDICE = VERSION_DICE_FULL;

  // Execute DICE core
  return DiceCore();
}
