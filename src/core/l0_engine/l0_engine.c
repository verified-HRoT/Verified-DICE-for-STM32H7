/*
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root.
 */

// Derived from: https://github.com/Microsoft/RIoT

#include <stdint.h>
#include <string.h>

#include "common/cert_constants.h"
#include "common/logger.h"
#include "common/machine.h"
#include "common/rng.h"
#include "global/version.h"

#include "LowStar_Printf.h"
#include "kremlin/internal/types.h"
#include "kremlin/lowstar_endianness.h"

#include "ASN1_X509.h"
#include "L0_Declassify.h"
#include "L0_Helpers.h"
#include "L0_X509_AliasKeyCRT.h"
#include "L0_X509_AliasKeyTBS.h"
#include "L0_X509_Base.h"
#include "L0_X509_DeviceIDCRI.h"
#include "L0_X509_DeviceIDCSR.h"

#include "Hacl_Lib.h"
#include "L0_Core.h"

#include "l0_engine.h"

static void L0Core(uint8_t *l1_base, const size_t l1_size,
                   l0_params_t *l0_params, l1_params_t *l1_params) {
  if ((l1_base == NULL) || (l0_params == NULL) || (l1_params == NULL)) {
    HW_LOG_FAIL("L0: Invalid arguments");
    return;
  }

  /* Common Inputs */
  uint8_t *cdi = l0_params->CDI.data;

  uint8_t fwid[32U] = {0U};
  Hacl_Hash_SHA2_hash_256(l1_base, (uint32_t)l1_size, fwid);

  uint8_t *deviceID_lbl = (uint8_t *)L0_LABEL_IDENTITY;
  uint32_t deviceID_lbl_len = strlen(L0_LABEL_IDENTITY);
  KRML_CHECK_SIZE(sizeof(uint8_t), deviceID_lbl_len);

  uint8_t *aliasKey_lbl = (uint8_t *)L0_LABEL_ALIAS;
  uint32_t aliasKey_lbl_len = strlen(L0_LABEL_ALIAS);
  KRML_CHECK_SIZE(sizeof(uint8_t), aliasKey_lbl_len);

  /* DeviceID CSR Inputs */
  int32_t ku0 = (int32_t)0x04;
  int32_t deviceIDCSR_version = (int32_t)2;

  uint32_t deviceIDCSR_s_common_len = strlen(DEVICE_SUBJECT_COMMON_NAME);
  uint8_t *deviceIDCSR_s_common_buf = (uint8_t *)DEVICE_SUBJECT_COMMON_NAME;
  uint32_t deviceIDCSR_s_org_len = strlen(DEVICE_SUBJECT_ORGANIZATION);
  uint8_t *deviceIDCSR_s_org_buf = (uint8_t *)DEVICE_SUBJECT_ORGANIZATION;
  uint32_t deviceIDCSR_s_country_len = strlen(DEVICE_SUBJECT_COUNTRY);
  uint8_t *deviceIDCSR_s_country_buf = (uint8_t *)DEVICE_SUBJECT_COUNTRY;

  character_string_t deviceIDCSR_s_common = {
      .fst = deviceIDCSR_s_common_len,
      .snd = FStar_Bytes_of_buffer(deviceIDCSR_s_common_len,
                                   (uint8_t *)deviceIDCSR_s_common_buf)};
  character_string_t deviceIDCSR_s_org = {
      .fst = deviceIDCSR_s_org_len,
      .snd = FStar_Bytes_of_buffer(deviceIDCSR_s_org_len,
                                   (uint8_t *)deviceIDCSR_s_org_buf)};
  character_string_t deviceIDCSR_s_country = {
      .fst = deviceIDCSR_s_country_len,
      .snd = FStar_Bytes_of_buffer(deviceIDCSR_s_country_len,
                                   (uint8_t *)deviceIDCSR_s_country_buf)};
  deviceIDCSR_ingredients_t deviceIDCSR_ingredients = {
      .deviceIDCSR_ku = ku0,
      .deviceIDCSR_version = deviceIDCSR_version,
      .deviceIDCSR_s_common = deviceIDCSR_s_common,
      .deviceIDCSR_s_org = deviceIDCSR_s_org,
      .deviceIDCSR_s_country = deviceIDCSR_s_country};

  /* AliasKey Crt Inputs */
  int32_t ku = (int32_t)0x04;
  int32_t l0_version = L0_VERSION;
  int32_t aliasKeyCrt_version = (int32_t)2;

  uint32_t aliasKeyCrt_serialNumber_len = (uint32_t)8U;

  uint8_t aliasKeyCrt_serialNumber_buf[8U] = {
      (uint8_t)0x27U, (uint8_t)0x48U, (uint8_t)0x4BU, (uint8_t)0xD6U,
      (uint8_t)0xF2U, (uint8_t)0x10U, (uint8_t)0xB4U, (uint8_t)0xDAU};

  octet_string_t aliasKeyCrt_serialNumber = {
      .len = aliasKeyCrt_serialNumber_len,
      .s = FStar_Bytes_of_buffer(aliasKeyCrt_serialNumber_len,
                                 (uint8_t *)aliasKeyCrt_serialNumber_buf)};

  uint32_t aliasKeyCrt_s_common_len = strlen(ALIAS_SUBJECT_COMMON_NAME);
  uint8_t *aliasKeyCrt_s_common_buf = (uint8_t *)ALIAS_SUBJECT_COMMON_NAME;
  uint32_t aliasKeyCrt_s_org_len = strlen(ALIAS_SUBJECT_ORGANIZATION);
  uint8_t *aliasKeyCrt_s_org_buf = (uint8_t *)ALIAS_SUBJECT_ORGANIZATION;
  uint32_t aliasKeyCrt_s_country_len = strlen(ALIAS_SUBJECT_COUNTRY);
  uint8_t *aliasKeyCrt_s_country_buf = (uint8_t *)ALIAS_SUBJECT_COUNTRY;
  uint32_t aliasKeyCrt_i_common_len = strlen(DEVICE_SUBJECT_COMMON_NAME);
  uint8_t *aliasKeyCrt_i_common_buf = (uint8_t *)DEVICE_SUBJECT_COMMON_NAME;
  uint32_t aliasKeyCrt_i_org_len = strlen(DEVICE_SUBJECT_ORGANIZATION);
  uint8_t *aliasKeyCrt_i_org_buf = (uint8_t *)DEVICE_SUBJECT_ORGANIZATION;
  uint32_t aliasKeyCrt_i_country_len = strlen(DEVICE_SUBJECT_COUNTRY);
  uint8_t *aliasKeyCrt_i_country_buf = (uint8_t *)DEVICE_SUBJECT_COUNTRY;

  /* AliasKey Crt Subject Names */
  character_string_t aliasKeyCrt_s_common = {
      .fst = aliasKeyCrt_s_common_len,
      .snd = FStar_Bytes_of_buffer(aliasKeyCrt_s_common_len,
                                   (uint8_t *)aliasKeyCrt_s_common_buf)};
  character_string_t aliasKeyCrt_s_org = {
      .fst = aliasKeyCrt_s_org_len,
      .snd = FStar_Bytes_of_buffer(aliasKeyCrt_s_org_len,
                                   (uint8_t *)aliasKeyCrt_s_org_buf)};
  character_string_t aliasKeyCrt_s_country = {
      .fst = aliasKeyCrt_s_country_len,
      .snd = FStar_Bytes_of_buffer(aliasKeyCrt_s_country_len,
                                   (uint8_t *)aliasKeyCrt_s_country_buf)};

  /* AliasKey Crt Issuer Names */
  character_string_t aliasKeyCrt_i_common = {
      .fst = aliasKeyCrt_i_common_len,
      .snd = FStar_Bytes_of_buffer(aliasKeyCrt_i_common_len,
                                   (uint8_t *)aliasKeyCrt_i_common_buf)};
  character_string_t aliasKeyCrt_i_org = {
      .fst = aliasKeyCrt_i_org_len,
      .snd = FStar_Bytes_of_buffer(aliasKeyCrt_i_org_len,
                                   (uint8_t *)aliasKeyCrt_i_org_buf)};
  character_string_t aliasKeyCrt_i_country = {
      .fst = aliasKeyCrt_i_country_len,
      .snd = FStar_Bytes_of_buffer(aliasKeyCrt_i_country_len,
                                   (uint8_t *)aliasKeyCrt_i_country_buf)};

  FStar_Bytes_bytes notBefore = FStar_Bytes_of_buffer(
      (uint32_t)13U, (uint8_t *)x509_validity_notBefore_default_buffer);
  FStar_Bytes_bytes notAfter = FStar_Bytes_of_buffer(
      (uint32_t)15U, (uint8_t *)x509_validity_notAfter_default_buffer);

  aliasKeyCRT_ingredients_t aliasKeyCRT_ingredients = {
      .aliasKeyCrt_version = aliasKeyCrt_version,
      .aliasKeyCrt_serialNumber = aliasKeyCrt_serialNumber,
      .aliasKeyCrt_i_common = aliasKeyCrt_i_common,
      .aliasKeyCrt_i_org = aliasKeyCrt_i_org,
      .aliasKeyCrt_i_country = aliasKeyCrt_i_country,
      .aliasKeyCrt_notBefore = notBefore,
      .aliasKeyCrt_notAfter = notAfter,
      .aliasKeyCrt_s_common = aliasKeyCrt_s_common,
      .aliasKeyCrt_s_org = aliasKeyCrt_s_org,
      .aliasKeyCrt_s_country = aliasKeyCrt_s_country,
      .aliasKeyCrt_ku = ku,
      .aliasKeyCrt_l0_version = l0_version};

  /* Outputs */
  uint32_t deviceIDCSR_len = len_of_deviceIDCSR(
      len_of_deviceIDCRI(deviceIDCSR_ingredients.deviceIDCSR_version,
                         deviceIDCSR_ingredients.deviceIDCSR_s_common,
                         deviceIDCSR_ingredients.deviceIDCSR_s_org,
                         deviceIDCSR_ingredients.deviceIDCSR_s_country));
  KRML_CHECK_SIZE(sizeof(uint8_t), deviceIDCSR_len);

  l1_params->DeviceIDCsrLen = deviceIDCSR_len;

  uint8_t *deviceIDCSR_buf = (uint8_t *)l1_params->DeviceIDCsr;

  uint8_t *deviceID_pub = l1_params->DeviceIDPub.data;

  uint32_t aliasKeyCRT_len = len_of_aliasKeyCRT(
      len_of_aliasKeyTBS(aliasKeyCRT_ingredients.aliasKeyCrt_serialNumber,
                         aliasKeyCRT_ingredients.aliasKeyCrt_i_common,
                         aliasKeyCRT_ingredients.aliasKeyCrt_i_org,
                         aliasKeyCRT_ingredients.aliasKeyCrt_i_country,
                         aliasKeyCRT_ingredients.aliasKeyCrt_s_common,
                         aliasKeyCRT_ingredients.aliasKeyCrt_s_org,
                         aliasKeyCRT_ingredients.aliasKeyCrt_s_country,
                         aliasKeyCRT_ingredients.aliasKeyCrt_l0_version));
  KRML_CHECK_SIZE(sizeof(uint8_t), aliasKeyCRT_len);

  l1_params->AliasKeyCertLen = aliasKeyCRT_len;

  uint8_t *aliasKeyCRT_buf = (uint8_t *)l1_params->AliasKeyCert;
  uint8_t *aliasKey_pub = l1_params->AliasKeyPub.data;
  uint8_t *aliasKey_priv = l1_params->AliasKeyPriv.data;

  /* Call l0 main function */
  HW_LOG_DEBUG("Enter L0");

  l0(cdi, fwid, deviceID_lbl_len, deviceID_lbl, aliasKey_lbl_len, aliasKey_lbl,
     deviceIDCSR_ingredients, aliasKeyCRT_ingredients, deviceID_pub,
     aliasKey_pub, aliasKey_priv, l1_params->DeviceIDCsrLen, deviceIDCSR_buf,
     l1_params->AliasKeyCertLen, aliasKeyCRT_buf);

  HW_LOG_DEBUG("Exit L0");

  // Can't write files on the device. Uncomment for running it on host.

  // uint8_t aliasKey_priv_pub[32U];
  // memset(aliasKey_priv_pub, 0U, (uint32_t)32U * sizeof (uint8_t));
  // RIoT_Declassify_declassify_secret_buffer((uint32_t)32U, aliasKey_priv,
  // aliasKey_priv_pub); write_out("DeviceIDPublicKey.hex", aliasKey_pub,
  // (uint32_t)32U); write_out("AliasKeyPublicKey.hex", aliasKey_pub,
  // (uint32_t)32U); write_out("AliasKeyPrivateKey.hex", aliasKey_priv_pub,
  // (uint32_t)32U); write_out("DeviceIDCSR.hex", deviceIDCSR_buf,
  // deviceIDCSR_len); write_out("AliasKeyCrt.hex", aliasKeyCRT_buf,
  // aliasKeyCRT_len);
}

EXTERNAL_C void L0HandleArgs(uint8_t *l1_base, const size_t l1_size,
                             l0_params_t *l0_params, l1_params_t *l1_params) {
  // Pass the versions of DICE and L0 to L1
  l1_params->VersionDICE = l0_params->VersionDICE;
  l1_params->VersionL0 = VERSION_L0_FULL;

  // Forward HW measurement from DICE to L1
  memcpy(&(l1_params->HWMeasurement[0]), &(l0_params->HWMeasurement[0]),
         sizeof(l0_params->HWMeasurement));

  // Execute L0 core
  L0Core(l1_base, l1_size, l0_params, l1_params);
}
