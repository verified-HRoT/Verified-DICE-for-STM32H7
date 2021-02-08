/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

INCLUDE memmap_flash.ld
INCLUDE memmap_ram.ld

HEADER_SIZE = 0x400;

__IMAGE_START__ = __L0_IMAGE_START__ + HEADER_SIZE;
__IMAGE_END__   = __L0_IMAGE_END__;

INCLUDE STM32H753ZITx.ld
