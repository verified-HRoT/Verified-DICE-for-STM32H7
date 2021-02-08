/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

/*
 * Flash bank 1 (0x08000000 - 0x080FFFFF) (1M)
 * -------------------------------------------
 */

__FW_UPDATE_FLAGS_SIZE__ = 32 * 3; /* 3 flash words */

/*
 * The first 128 KB sector is used for DICE, and is protected with the
 * secure user area.
 */
__DICE_IMAGE_START__ = 0x08000000;
__DICE_IMAGE_SIZE__  = 128K;
__DICE_IMAGE_END__   = __DICE_IMAGE_START__ + __DICE_IMAGE_SIZE__ - 1;

/* There is 1 KB allocated to the key manifest */
__KEY_MANIFEST_START__ = __DICE_IMAGE_END__ + 1;
__KEY_MANIFEST_SIZE__  = 1K;
__KEY_MANIFEST_END__   = __KEY_MANIFEST_START__ + __KEY_MANIFEST_SIZE__ - 1;

/* There are 255 KB allocated to the L0 image */
__L0_IMAGE_START__ = __KEY_MANIFEST_END__ + 1;
__L0_IMAGE_SIZE__  = 255K;
__L0_IMAGE_END__   = __L0_IMAGE_START__ + __L0_IMAGE_SIZE__ - 1;

/* The other 512 KB are allocated to the L1 image */
__L1_IMAGE_START__ = __L0_IMAGE_END__ + 1;
__L1_IMAGE_SIZE__  = 512K - __FW_UPDATE_FLAGS_SIZE__;
__L1_IMAGE_END__   = __L1_IMAGE_START__ + __L1_IMAGE_SIZE__ - 1;

/* Key manifest + L0 + L1 are the active firmware image (768 KB) */
__REGION_FW_ACTIVE_START__ = __KEY_MANIFEST_START__;
__REGION_FW_ACTIVE_END__   = __L1_IMAGE_END__;

/* The rest of bank 1 is unused for now */

