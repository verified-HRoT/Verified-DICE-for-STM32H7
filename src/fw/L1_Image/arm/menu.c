/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include <stdio.h>

#include <stm32h7xx_hal.h>
#include <stm32h7xx_ll_utils.h>
#include <stm32h7xx_nucleo_144.h>

#include "common/logger.h"
#include "core/l1_params.h"
#include "fw/common/arm/uart.h"
#include "fw/common/flash.h"
#include "fw/common/image_header.h"
#include "fw/common/memmap.h"
#include "fw/common/shared_mem_volatile.h"
#include "fw/common/watchdog.h"

#include "../tests.h"

static void PrintMainMenu(void) {
  HW_LOG_DEBUG("Main Menu");
  HW_LOG_DEBUG("=========");
  HW_LOG_DEBUG(" 1: Test Protections");
  HW_LOG_DEBUG(" 2: Print Artifacts (AliasCRT, DeviceIDCSR)");
}

static void PrintTests(void) {
  HW_LOG_DEBUG("");
  HW_LOG_DEBUG("Tests Menu");
  HW_LOG_DEBUG("==========");
  HW_LOG_DEBUG(" 1: Software Watchdog");
  HW_LOG_DEBUG(" 2: Hardware Watchdog");
  HW_LOG_DEBUG(" 3: Corrupt active image");
  HW_LOG_DEBUG(" 0: Exit");
  HW_LOG_DEBUG("");
}

void Test_Watchdog_SW(void) {
  // A timeout between the software watchdog and hardware watchdog timeouts
  unsigned int timeout = TIMEOUT_HW_WATCHDOG_SECONDS - 1;

  HW_LOG_DEBUG("Test: Software Watchdog");
  HW_LOG_DEBUG("-----------------------");
  HW_LOG_DEBUG("");
  HW_LOG_DEBUG("Waiting for %u seconds. The board should reboot.", timeout);

  // Make sure that the hardware watchdog isn't triggered
  while (timeout > 0) {
    HAL_Delay(1000);
    HW_Watchdog_Reload();
    timeout--;
  }

  HW_LOG_DEBUG("    FAIL!");
  HW_LOG_DEBUG("");
}

void Test_Watchdog_HW(void) {
  unsigned int timeout = TIMEOUT_HW_WATCHDOG_SECONDS + 1;

  HW_LOG_DEBUG("Test: Hardware Watchdog");
  HW_LOG_DEBUG("-----------------------");
  HW_LOG_DEBUG("");
  HW_LOG_DEBUG("Waiting for %u seconds. The board should reboot.", timeout);

  // Make sure that the software watchdog isn't triggered
  while (timeout > 0) {
    HAL_Delay(1000);
    SW_Watchdog_Reload();
    timeout--;
  }

  HW_LOG_DEBUG("    FAIL!");
  HW_LOG_DEBUG("");
}

void Test_CorruptActiveImage(void) {
  uint8_t buffer[128];
  uint32_t address = (uintptr_t)GetL0ImageStart() + FIRMWARE_IMAGE_OFFSET;

  // Fill with zeros to corrupt all bits of the destination
  memset(buffer, 0, sizeof(buffer));

  HW_LOG_DEBUG("Test: Corrupt Active Image");
  HW_LOG_DEBUG("--------------------------");
  HW_LOG_DEBUG("");

  int ret = Flash_Write((void *)address, (void *)&buffer, sizeof(buffer));
  if (ret != 0) {
    HW_LOG_DEBUG("Failed to write to flash!");
    HW_LOG_DEBUG("    FAIL!");
  }

  HW_LOG_DEBUG("Next reboot should trigger a boot of the recovery image.");
  HW_LOG_DEBUG("");

  NVIC_SystemReset();
}

static void RunTests(void) {
  uint8_t key = 0;

  PrintTests();

  while (1) {
    Watchdog_Reload();

    UART_Flush();

    if (UART_Receive(&key, 1U, 100) != HAL_OK)
      continue;

    switch (key) {
    case '1':
      Test_Watchdog_SW();
      break;
    case '2':
      Test_Watchdog_HW();
      break;
    case '3':
      Test_CorruptActiveImage();
      break;
    case '0':
      return;
    default:
      break;
    }

    PrintTests();
  }
}

static void PrintArtifacts(void) {
  // Get L1 Certs and CSR address location
  const l1_params_t *l1_params = (const l1_params_t *)GetShared_L0_L1();

  HW_LOG_DEBUG("AliasKey CRT in DER format (hex)");
  for (uint32_t i = 0; i < l1_params->AliasKeyCertLen; i++) {
    printf("%02x", l1_params->AliasKeyCert[i]);
  }

  HW_LOG_DEBUG("\nDeviceID CSR in DER format (hex)");
  for (uint32_t i = 0; i < l1_params->DeviceIDCsrLen; i++) {
    printf("%02x", l1_params->DeviceIDCsr[i]);
  }
}

void L1_Menu(void) {
  uint32_t package = LL_GetPackageType();
  const char *package_name = "";

  if (package == LL_UTILS_PACKAGETYPE_TQFP144) {
    package_name = "Nucleo board";
  } else if (package == LL_UTILS_PACKAGETYPE_TQFP176_UFBGA176) {
    package_name = "Vendor board";
  } else {
    package_name = "Unknown";
  }

  HW_LOG_DEBUG("Package type: %u (%s)", (unsigned int)package, package_name);

  // Only enable the emergency exit if the FW is running on a Nucleo board
  if (package != LL_UTILS_PACKAGETYPE_TQFP144) {
    return;
  }

  // Start tests menu

  PrintMainMenu();

  while (1) {
    Watchdog_Reload();

    // Clean the input path
    UART_Flush();

    uint8_t key = 0U;
    // Receive key
    if (UART_Receive(&key, 1U, 100) == HAL_OK) {
      switch (key) {
      case '1':
        RunTests();
        break;
      case '2':
        PrintArtifacts();
        break;
      default:
        HW_LOG_DEBUG("Invalid Number !");
        break;
      }

      PrintMainMenu();
    }
  }
}
