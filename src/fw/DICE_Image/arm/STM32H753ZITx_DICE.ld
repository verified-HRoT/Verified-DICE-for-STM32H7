/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

INCLUDE memmap_flash.ld
INCLUDE memmap_ram.ld

__IMAGE_START__ = __DICE_IMAGE_START__;
__IMAGE_END__   = __DICE_IMAGE_END__;

INCLUDE STM32H753ZITx.ld
