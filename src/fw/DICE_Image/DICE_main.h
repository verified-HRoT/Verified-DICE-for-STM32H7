/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

// It returns 0 on a successful run, or an error code. On success, the caller
// should jump to L0. On failure the caller should just hang.
int DICE_main(void);
