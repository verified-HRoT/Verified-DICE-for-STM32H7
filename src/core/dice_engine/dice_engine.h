/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

#include "common/machine.h"
#include "core/l0_params.h"

EXTERNAL_C_BEGIN
// Function that performs the DICE core operations.
EXTERNAL_C int DiceCore();

// Function that performs the DICE core operations and handles the arguments
// passed to L0. It gets as inputs the HW Measurement string, and a struct where
// it stores the results.
int DICEHandleArgsAndDice(const uint8_t *hw_measurement,
                          size_t hw_measurement_size, l0_params_t *l0_params);

EXTERNAL_C_END
