/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include <stm32h7xx_hal.h>

#include "common/logger.h"
#include "fw/DICE_Image/arm/rss_definitions.h"
#include "fw/common/arm/memmap_h7.h"
#include "fw/common/image_header.h"

// Protect DICE and Key manifest
#define RSS_BANK1_AREA_START DICE_IMAGE_START
#define RSS_BANK1_AREA_END (L0_IMAGE_START - 1)
#define RSS_BANK1_AREA_SIZE (RSS_BANK1_AREA_END - RSS_BANK1_AREA_START + 1)

// Values as read from the OB struct. The protected area is:
//   (RSS_BANK1_AREA_START_OB) to (RSS_BANK1_AREA_END_OB + 0xFF)
#define RSS_BANK1_AREA_START_OB ((RSS_BANK1_AREA_START >> 8) & 0xFFF)
#define RSS_BANK1_AREA_END_OB ((RSS_BANK1_AREA_END >> 8) & 0xFFF)

// Expected values for all registers except for OPTSR_CUR
#define EXPECTED_PRAR_CUR1 (0x000000FFu)
#define EXPECTED_PRAR_CUR2 (0x000000FFu)
#define EXPECTED_SCAR_CUR1 (0x82030000u)
#define EXPECTED_SCAR_CUR2 (0x00000FFFu)
#define EXPECTED_WPSN_CUR1 (0x000000FFu)
#define EXPECTED_WPSN_CUR2 (0x000000FEu)
#define EXPECTED_BOOT_CUR (0x08000800u)

int CheckProtections(void) {
  int ret = 0;

  // All the option bytes registers below are uint32_t

  HW_LOG_TRACE("Option bytes:\n"
               "\n"
               "OPTSR_CUR : 0x%08X\n"
               "PRAR_CUR1 : 0x%08X\n"
               "PRAR_CUR2 : 0x%08X\n"
               "SCAR_CUR1 : 0x%08X\n"
               "SCAR_CUR2 : 0x%08X\n"
               "WPSN_CUR1 : 0x%08X\n"
               "WPSN_CUR2 : 0x%08X\n"
               "BOOT_CUR  : 0x%08X\n"
               "\n",
               FLASH->OPTSR_CUR, FLASH->PRAR_CUR1, FLASH->PRAR_CUR2,
               FLASH->SCAR_CUR1, FLASH->SCAR_CUR2, FLASH->WPSN_CUR1,
               FLASH->WPSN_CUR2, FLASH->BOOT_CUR);

  // FOPTSR_PRG has some fields that don't matter for security
  // ---------------------------------------------------------

  // Bits that we don't care about in this register:
  const uint32_t ignored_mask = 0x3FD900EF;

  // Expected values for RDP level 1 or 2
  const uint32_t expected_rdp1 = 0x0026BB10;
  const uint32_t expected_rdp2 = 0x0026CC10;

  // Read register and mask out the fields we don't care about
  const uint32_t foptsr_prg_masked = FLASH->OPTSR_CUR & ~ignored_mask;

  if (foptsr_prg_masked == expected_rdp1) {
    HW_LOG_INFO("RDP level 1 set");
  } else if (foptsr_prg_masked == expected_rdp2) {
    HW_LOG_INFO("RDP level 2 set");
  } else {
    HW_LOG_FAIL("OPTSR_CUR: 0x%08X (not valid)", FLASH->OPTSR_CUR);
    ret = -1;
  }

  // The values of the other registers must match exactly
  // ----------------------------------------------------

  // The following check can't be done at compilation time because it depends
  // on some addresses that are calculated at link time.

  uint32_t calculated_scar_cur1 =
      (1 << 31) | RSS_BANK1_AREA_START_OB | (RSS_BANK1_AREA_END_OB << 16);

  if (EXPECTED_SCAR_CUR1 != calculated_scar_cur1) {
    // If the size of DICE changes, or the size of the header of L0, this may
    // change and EXPECTED_SCAR_CUR1 will need to be changed.
    HW_LOG_FAIL("The value of EXPECTED_SCAR_CUR1 is outdated: 0x%08lX",
                calculated_scar_cur1);
    // This is a programming error. It is important to update the value, so
    // don't let the execution continue.
    while (1)
      ;
  }

#if HW_SET_RSS_PROTECTIONS_AT_RUNTIME
  // This code is needed in Nucleo boards to set up the Secure User Area.
  // Check the instructions in "Test protections" in src/fw/readme.md

  if (FLASH->SCAR_CUR1 != EXPECTED_SCAR_CUR1) {
    // The secure memory area is not set, try to configure it.

    rss_secure_area_t area[RSS_NUM_SECURE_AREAS];

    area[0].size = RSS_BANK1_AREA_SIZE;
    area[0].address = RSS_BANK1_AREA_START;
    area[0].remove_during_bank_erase = 1;

    area[1].size = 0;
    area[1].address = (uint32_t)NULL;
    area[1].remove_during_bank_erase = 1;

    HW_LOG_INFO("Setting up secure area: 0x%08lX - 0x%08lX. Resetting...",
                area[0].address, area[0].address + area[0].size - 1);

    // This call triggers a system reset
    RSS_TABLE->resetAndInitializeSecureAreas(1, area);

    // This shouldn't be reached. If it is, we are in a state in which we
    // shouldn't trust the stack, so just hang.
    while (1)
      ;
  }
#endif // HW_SET_RSS_PROTECTIONS_AT_RUNTIME

  struct {
    uint32_t current_value;
    uint32_t expected_value;
    char *name;
  } regs[] = {{FLASH->PRAR_CUR1, EXPECTED_PRAR_CUR1, "PRAR_CUR1"},
              {FLASH->PRAR_CUR2, EXPECTED_PRAR_CUR2, "PRAR_CUR2"},
              {FLASH->SCAR_CUR1, EXPECTED_SCAR_CUR1, "SCAR_CUR1"},
              {FLASH->SCAR_CUR2, EXPECTED_SCAR_CUR2, "SCAR_CUR2"},
              {FLASH->WPSN_CUR1, EXPECTED_WPSN_CUR1, "WPSN_CUR1"},
              {FLASH->WPSN_CUR2, EXPECTED_WPSN_CUR2, "WPSN_CUR2"},
              {FLASH->BOOT_CUR, EXPECTED_BOOT_CUR, "BOOT_CUR "},
              {0, 0, NULL}};

  size_t i = 0;
  while (1) {
    if (regs[i].name == NULL)
      break;

    if (regs[i].current_value != regs[i].expected_value) {
      HW_LOG_FAIL("%s: 0x%08X (expected 0x%08X)", regs[i].name,
                  regs[i].current_value, regs[i].expected_value);
      ret = -1;
    }

    i++;
  }

#if HW_IGNORE_PROTECTION_CHECKS
  // Return success no matter what happened earlier
  ret = 0;
#endif

  return ret;
}
