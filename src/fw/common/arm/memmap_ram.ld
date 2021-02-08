/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

/*
 * DTCM (0x20000000 - 0x2001FFFF) (128K)
 * -------------------------------------
 */

__DTCM_START__   = 0x20000000;
__DTCM_LENGTH__  = 128K;
__DTCM_END__     = __DTCM_START__ + __DTCM_LENGTH__ - 1;

/*
 * AXI SRAM (0x24000000 - 0x2407FFFF) (512K)
 * -----------------------------------------
 */

__AXI_SRAM_START__   = 0x24000000;
__AXI_SRAM_LENGTH__  = 512K;

/*
 * SRAM1 + SRAM2 + SRAM3 (0x30000000 - 0x30047FFF) (288K)
 * ------------------------------------------------------
 */

__SRAM123_START__   = 0x30000000;
__SRAM123_LENGTH__  = 288K;
