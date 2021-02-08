/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include "fw/common/option_bytes.h"
#include "common/machine.h"

void ReadOptionBytes(option_bytes *ob) {
  // There are no option bytes in the host. Just fill the buffer with zeroes.
  platform_zeroize(&(ob->data[0]), sizeof(option_bytes));
}
