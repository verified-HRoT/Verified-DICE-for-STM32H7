/*
 * Copyright(c) 2020 Microsoft. All rights reserved.
 */

// Test suite for the bootloader firmware, including authentication.
//
// Note that this code doesn't generate test private keys, they need to be
// provided in the command line as arguments when starting the tests:
//
//     fw_tests KEY_MANIFEST_PRIV_PEM.txt L0_PRIV_PEM.txt L1_PRIV_PEM.txt

#include "common/Blob.h"
#include "common/logger.h"
#include "common/machine.h"
#include "common/rng.h"
#include "common/testing.h"
#include "crypto/crypto.h"
#include "crypto/crypto_files.h"
#include "external/cxxopts.h"
#include "fw/DICE_Image/host/DICE_main_host.h"
#include "fw/DICE_Image/reset.h"
#include "fw/DICE_Image/secret.h"
#include "fw/L0_Image/host/l0_main_host.h"
#include "fw/L1_Image/host/l1_main_host.h"
#include "fw/common/firmware_auth.h"
#include "fw/common/image_auth.h"
#include "fw/common/image_header.h"
#include "fw/common/memmap.h"
#include "fw/common/shared_mem_persistent.h"
#include "fw/headertool/header_utils.h"

static fw_pub_key pub_key_manifest, pub_key_l0, pub_key_l1;
static fw_priv_key priv_key_manifest, priv_key_l0, priv_key_l1;

class KeyManifest {
private:
  key_manifest_t manifest = {};

public:
  int Generate() {
    manifest.version = KEY_MANIFEST_VERSION;
    memcpy(&(manifest.l0_pub_key.data[0]), &(pub_key_l0.data[0]),
           sizeof(fw_pub_key));
    memcpy(&(manifest.l1_pub_key.data[0]), &(pub_key_l1.data[0]),
           sizeof(fw_pub_key));

    const void *data = &manifest;
    constexpr size_t size = sizeof(manifest) - sizeof(manifest.signature);

    const int ret =
        ED25519_Sign(&manifest.signature, data, size, &priv_key_manifest);
    if (ret != 0) {
      LOG_FAIL_FMT("ED25519_Sign(): %d\n", ret);
      return -1;
    }

    return 0;
  }

  const void *data() { return &manifest; }

  size_t size() { return sizeof(manifest); }
};

static KeyManifest keyManifest;

class FwUpdate : public Blob<FW_UPDATE_TOTAL_SIZE> {
  using Blob::Blob;
  uint32_t l0_size = 0;
  uint32_t l1_size = 0;
  uint32_t l1_offset = 0;

public:
  void SetImageSizes(uint32_t l0_image_size, uint32_t l1_image_size) {
    assert(l0_image_size > 0 && l0_image_size <= L0_IMAGE_SIZE);
    assert(l1_image_size > 0 && l1_image_size <= L1_IMAGE_SIZE);

    l0_size = l0_image_size;
    l1_size = l1_image_size;

    l1_offset = L0_TOTAL_SIZE;
  }

  uint8_t *GetL0() { return data() + KEY_MANIFEST_SIZE; }

  // This is just useful to set the offset to an invalid value
  void SetL1Offset(uint32_t offset) { l1_offset = offset; }

  int Generate(void) {
    // Clear everything
    memset(data(), 0, length());

    // Copy key manifest
    memcpy(data(), keyManifest.data(), keyManifest.size());

    constexpr uint32_t l0_base = KEY_MANIFEST_SIZE;
    constexpr uint32_t l1_base = l0_base + L0_TOTAL_SIZE;

    // Fill L0 and L1 with random data
    RNG_Rand(data() + l0_base + IMAGE_HEADER_SIZE, l0_size);
    RNG_Rand(data() + l1_base + IMAGE_HEADER_SIZE, l1_size);

    // Pointers to L0 and L1
    image_header_t *l0_header = (image_header_t *)&(data()[l0_base]);
    void *l0_image = &(data()[l0_base + IMAGE_HEADER_SIZE]);
    image_header_t *l1_header = (image_header_t *)&(data()[l1_base]);
    void *l1_image = &(data()[l1_base + IMAGE_HEADER_SIZE]);

    int ret = 0;

    // Generate L0 header
    ret |= image_header_build(l0_header, 1, l0_image, l0_size, l1_offset);
    ret |= image_header_sign(l0_header, &priv_key_l0);

    // Generate L1 header
    ret |= image_header_build(l1_header, 1, l1_image, l1_size, 0);
    ret |= image_header_sign(l1_header, &priv_key_l1);

    return ret;
  }

  int Generate(uint32_t l0_image_size, uint32_t l1_image_size) {
    SetImageSizes(l0_image_size, l1_image_size);
    return Generate();
  }

  void WriteRandomData(size_t offset, size_t size) {
    RNG_Rand(data() + offset, size);
  }
};

class FwComplete : public Blob<FW_COMPLETE_TOTAL_SIZE> {
  using Blob::Blob;

public:
  int Generate(uint32_t l0_image_size, uint32_t l1_image_size) {
    assert(l0_image_size > 0 && l0_image_size <= L0_IMAGE_SIZE);
    assert(l1_image_size > 0 && l1_image_size <= L1_IMAGE_SIZE);

    constexpr uint32_t dice_base = 0;
    constexpr uint32_t manifest_base = DICE_TOTAL_SIZE;
    constexpr uint32_t l0_base = manifest_base + KEY_MANIFEST_SIZE;
    constexpr uint32_t l1_base = l0_base + L0_TOTAL_SIZE;

    // Clear everything
    memset(data(), 0, length());

    // Fill DICE with random data
    RNG_Rand(data() + dice_base, DICE_TOTAL_SIZE);

    // Copy key manifest
    memcpy(data() + manifest_base, keyManifest.data(), keyManifest.size());

    // Fill L0 and L1 with random data
    RNG_Rand(data() + l0_base + IMAGE_HEADER_SIZE, l0_image_size);
    RNG_Rand(data() + l1_base + IMAGE_HEADER_SIZE, l1_image_size);

    // Pointers to L0 and L1
    image_header_t *l0_header = (image_header_t *)&(data()[l0_base]);
    void *l0_image = &(data()[l0_base + IMAGE_HEADER_SIZE]);
    image_header_t *l1_header = (image_header_t *)&(data()[l1_base]);
    void *l1_image = &(data()[l1_base + IMAGE_HEADER_SIZE]);

    int ret = 0;

    // Generate L0 header
    ret |= image_header_build(l0_header, 1, l0_image, l0_image_size,
                              L0_TOTAL_SIZE);
    ret |= image_header_sign(l0_header, &priv_key_l0);

    // Generate L1 header
    ret |= image_header_build(l1_header, 1, l1_image, l1_image_size, 0);
    ret |= image_header_sign(l1_header, &priv_key_l1);

    return ret;
  }

  const image_header_t *l0_header() {
    constexpr uint32_t manifest_base = DICE_TOTAL_SIZE;
    constexpr uint32_t l0_base = manifest_base + KEY_MANIFEST_SIZE;
    return (image_header_t *)&(data()[l0_base]);
  }
};

// Fake firmware that is as big as the biggest firmware we can have
FwComplete complete_fw;
FwUpdate update_fw;
FwUpdate corrupted_fw;
FwUpdate test_fw;

static void ClearMemory(void) {
  // Reset value of flash is 0xFF
  memset(GetFlashStart(), 0xFF, TOTAL_FLASH_SIZE);

  // Reset RAM
  RNG_Rand(static_cast<uint8_t *>(GetSharedRamStart()), GetSharedRamSize());
  RNG_Rand(static_cast<uint8_t *>(GetPersistentRamStart()),
           GetPersistentRamSize());

  // Set a sensible reset reason
  ResetReason_Set(RESET_REASON_HW_RESET);
}

static void ClearUDS(void) {
  hw_uds UDS = {0};
  memset(&UDS, 0xFF, sizeof(UDS)); // Reset value is 0xFF
  GT_EXPECT_EQ(0, DeviceSecretSet(&UDS));
}

namespace FwVerifyKeyManifest {
void CorruptImage(FwUpdate &fw, size_t offset, size_t size) {
  for (size_t i = 0; i < size; i++) {
    uint8_t val = (uint8_t)~fw.array().at(offset + i);
    fw.array().at(offset + i) = val;
  }
}

DEF_TEST(FwVerifyKeyManifest, Success) {
  // Authenticate base image successfully as reference
  GT_EXPECT_EQ(VerifyKeyManifest(update_fw.data(), &pub_key_manifest),
               VERIFY_SUCCESS);
}

DEF_TEST(FwVerifyKeyManifest, IncorrectKey) {
  // Try to authenticate with incorrect key
  fw_pub_key wrongkey = {0};
  wrongkey.data[0] = 0x40; // The format must be correct
  GT_EXPECT_EQ(VerifyKeyManifest(update_fw.data(), &wrongkey),
               VERIFY_ERROR_SIGNATURE);
}

DEF_TEST(FwVerifyKeyManifest, CorruptVersion) {
  // Corrupt version
  corrupted_fw = update_fw;
  CorruptImage(corrupted_fw, offsetof(key_manifest_t, version), 2);
  GT_EXPECT_EQ(VerifyKeyManifest(corrupted_fw.data(), &pub_key_manifest),
               VERIFY_ERROR_VERSION);
}

DEF_TEST(FwVerifyKeyManifest, CorruptL0Key) {
  // Corrupt L0 key
  corrupted_fw = update_fw;
  CorruptImage(corrupted_fw, offsetof(key_manifest_t, l0_pub_key), 2);
  GT_EXPECT_EQ(VerifyKeyManifest(corrupted_fw.data(), &pub_key_manifest),
               VERIFY_ERROR_SIGNATURE);
}

DEF_TEST(FwVerifyKeyManifest, CorruptL1Key) {
  // Corrupt L1 key
  corrupted_fw = update_fw;
  CorruptImage(corrupted_fw, offsetof(key_manifest_t, l1_pub_key), 2);
  GT_EXPECT_EQ(VerifyKeyManifest(corrupted_fw.data(), &pub_key_manifest),
               VERIFY_ERROR_SIGNATURE);
}

DEF_TEST(FwVerifyKeyManifest, CorruptSignature) {
  // Corrupt signature
  corrupted_fw = update_fw;
  CorruptImage(corrupted_fw, offsetof(key_manifest_t, signature), 2);
  GT_EXPECT_EQ(VerifyKeyManifest(corrupted_fw.data(), &pub_key_manifest),
               VERIFY_ERROR_SIGNATURE);
}
} // namespace FwVerifyKeyManifest

namespace FwVerifyImage {
void CorruptImage(FwUpdate &fw, size_t offset, size_t size) {
  for (size_t i = 0; i < size; i++) {
    uint8_t val = (uint8_t)~fw.array().at(KEY_MANIFEST_SIZE + offset + i);
    fw.array().at(KEY_MANIFEST_SIZE + offset + i) = val;
  }
}

DEF_TEST(FwVerifyImage, Success) {
  // Authenticate base image successfully as reference
  GT_EXPECT_EQ(VerifyImage(update_fw.GetL0(), &pub_key_l0), VERIFY_SUCCESS);
}

DEF_TEST(FwVerifyImage, IncorrectKey) {
  // Try to authenticate with incorrect key
  fw_pub_key wrongkey = {0};
  wrongkey.data[0] = 0x40; // The format must be correct
  GT_EXPECT_EQ(VerifyImage(update_fw.GetL0(), &wrongkey),
               VERIFY_ERROR_SIGNATURE);
}

DEF_TEST(FwVerifyImage, CorruptMagic) {
  // Corrupt magic number
  corrupted_fw = update_fw;
  CorruptImage(corrupted_fw, offsetof(image_header_t, magic), 2);
  GT_EXPECT_EQ(VerifyImage(corrupted_fw.GetL0(), &pub_key_l0),
               VERIFY_ERROR_MAGIC);
}

DEF_TEST(FwVerifyImage, SizeIs0) {
  // Size is 0
  corrupted_fw = update_fw;

  image_header_t *l0_header = (image_header_t *)corrupted_fw.GetL0();
  l0_header->fw_size = 0;

  GT_EXPECT_EQ(VerifyImage(corrupted_fw.GetL0(), &pub_key_l0),
               VERIFY_ERROR_SIZE);
}

DEF_TEST(FwVerifyImage, SizeIsTooBig) {
  // Size is too big
  corrupted_fw = update_fw;

  image_header_t *l0_header = (image_header_t *)corrupted_fw.GetL0();
  l0_header->fw_size = UINT32_MAX;

  GT_EXPECT_EQ(VerifyImage(corrupted_fw.GetL0(), &pub_key_l0),
               VERIFY_ERROR_SIZE);
}

DEF_TEST(FwVerifyImage, CorruptSize) {
  // Corrupt size
  corrupted_fw = update_fw;
  CorruptImage(corrupted_fw, offsetof(image_header_t, fw_size), 4);
  GT_EXPECT_EQ(VerifyImage(corrupted_fw.GetL0(), &pub_key_l0),
               VERIFY_ERROR_SIZE);
}

DEF_TEST(FwVerifyImage, CorruptHash) {
  // Corrupt hash
  corrupted_fw = update_fw;
  CorruptImage(corrupted_fw, offsetof(image_header_t, fw_hash), 1);
  GT_EXPECT_EQ(VerifyImage(corrupted_fw.GetL0(), &pub_key_l0),
               VERIFY_ERROR_HASH);
}

DEF_TEST(FwVerifyImage, CorruptFirmware) {
  // Corrupt firmware image itself
  corrupted_fw = update_fw;
  CorruptImage(corrupted_fw, FIRMWARE_IMAGE_OFFSET, 1);
  GT_EXPECT_EQ(VerifyImage(corrupted_fw.GetL0(), &pub_key_l0),
               VERIFY_ERROR_HASH);
}

DEF_TEST(FwVerifyImage, CorruptSignature) {
  // Corrupt signature
  corrupted_fw = update_fw;
  CorruptImage(corrupted_fw, offsetof(image_header_t, header_signature), 1);
  GT_EXPECT_EQ(VerifyImage(corrupted_fw.GetL0(), &pub_key_l0),
               VERIFY_ERROR_SIGNATURE);
}

DEF_TEST(FwVerifyImage, CorruptNextImageOffset) {
  // Corrupt next image offset
  corrupted_fw = update_fw;
  CorruptImage(corrupted_fw, offsetof(image_header_t, next_image_offset), 1);
  GT_EXPECT_EQ(VerifyImage(corrupted_fw.GetL0(), &pub_key_l0),
               VERIFY_ERROR_SIGNATURE);
}

DEF_TEST(FwVerifyImage, CorruptPadding) {
  // Corrupt padding so that header signature verification fails
  corrupted_fw = update_fw;
  CorruptImage(corrupted_fw, offsetof(image_header_t, padding), 1);
  GT_EXPECT_EQ(VerifyImage(corrupted_fw.GetL0(), &pub_key_l0),
               VERIFY_ERROR_SIGNATURE);
}
} // namespace FwVerifyImage

namespace FwAuthenticateFirmware {
DEF_TEST(FwAuthenticateFirmware, SuccessFullSpaceUsed) {
  // Authenticate base image successfully as reference
  test_fw.Generate(L0_IMAGE_SIZE, L1_IMAGE_SIZE);
  GT_EXPECT_EQ(AuthenticateFirmware(test_fw.data(), &pub_key_manifest),
               FW_AUTH_SUCCESS);
}

DEF_TEST(FwAuthenticateFirmware, SuccessHalfSpaceUsed) {
  // Authenticate base image successfully as reference
  test_fw.Generate(L0_IMAGE_SIZE / 2, L1_IMAGE_SIZE / 2);
  GT_EXPECT_EQ(AuthenticateFirmware(test_fw.data(), &pub_key_manifest),
               FW_AUTH_SUCCESS);
}

DEF_TEST(FwAuthenticateFirmware, CorruptKeyManifest) {
  // Corrupt Key Manifest

  test_fw.Generate(L0_IMAGE_SIZE / 2, L1_IMAGE_SIZE / 2);
  test_fw.WriteRandomData(10, 10);

  GT_EXPECT_EQ(AuthenticateFirmware(test_fw.data(), &pub_key_manifest),
               FW_AUTH_INVALID_MANIFEST);
}

DEF_TEST(FwAuthenticateFirmware, CorruptL0) {
  // Corrupt L0

  test_fw.Generate(L0_IMAGE_SIZE / 2, L1_IMAGE_SIZE / 2);
  test_fw.WriteRandomData(IMAGE_HEADER_SIZE + 10, 10);

  GT_EXPECT_EQ(AuthenticateFirmware(test_fw.data(), &pub_key_manifest),
               FW_AUTH_INVALID_L0);
}

DEF_TEST(FwAuthenticateFirmware, CorruptL1) {
  // Corrupt L1

  test_fw.Generate(L0_IMAGE_SIZE / 2, L1_IMAGE_SIZE / 2);
  test_fw.WriteRandomData(L0_TOTAL_SIZE + IMAGE_HEADER_SIZE + 10, 10);

  GT_EXPECT_EQ(AuthenticateFirmware(test_fw.data(), &pub_key_manifest),
               FW_AUTH_INVALID_L1);
}

DEF_TEST(FwAuthenticateFirmware, CorruptL0Header) {
  // Corrupt L0 header (more exhaustive tests done in FwVerifyImage)

  test_fw.Generate(L0_IMAGE_SIZE / 2, L1_IMAGE_SIZE / 2);
  test_fw.WriteRandomData(KEY_MANIFEST_SIZE, 10);

  GT_EXPECT_EQ(AuthenticateFirmware(test_fw.data(), &pub_key_manifest),
               FW_AUTH_INVALID_L0);
}

DEF_TEST(FwAuthenticateFirmware, CorruptL1Header) {
  // Corrupt L1 header (more exhaustive tests done in FwVerifyImage)

  test_fw.Generate(L0_IMAGE_SIZE / 2, L1_IMAGE_SIZE / 2);
  test_fw.WriteRandomData(KEY_MANIFEST_SIZE + L0_TOTAL_SIZE, 10);

  GT_EXPECT_EQ(AuthenticateFirmware(test_fw.data(), &pub_key_manifest),
               FW_AUTH_INVALID_L1);
}

DEF_TEST(FwAuthenticateFirmware, ExtraDataBetweenKeyManifestAndL0) {
  // Add extra data between Key manifest and L0

  test_fw.Generate(L0_IMAGE_SIZE / 2, L1_IMAGE_SIZE / 2);
  test_fw.WriteRandomData(sizeof(key_manifest_t), 10);

  GT_EXPECT_EQ(AuthenticateFirmware(test_fw.data(), &pub_key_manifest),
               FW_AUTH_EXTRA_DATA_MANIFEST_L0);
}

DEF_TEST(FwAuthenticateFirmware, ExtraDataBetweenL0AndL1) {
  // Add extra data between L0 and L1

  test_fw.Generate(L0_IMAGE_SIZE / 2, L1_IMAGE_SIZE / 2);
  test_fw.WriteRandomData(
      KEY_MANIFEST_SIZE + IMAGE_HEADER_SIZE + L0_IMAGE_SIZE / 2, 10);

  GT_EXPECT_EQ(AuthenticateFirmware(test_fw.data(), &pub_key_manifest),
               FW_AUTH_EXTRA_DATA_L0_L1);
}

DEF_TEST(FwAuthenticateFirmware, ExtraDataAfterL1) {
  // Add extra data after L1

  test_fw.Generate(L0_IMAGE_SIZE / 2, L1_IMAGE_SIZE / 2);
  test_fw.WriteRandomData(KEY_MANIFEST_SIZE + L0_TOTAL_SIZE +
                              IMAGE_HEADER_SIZE + L1_IMAGE_SIZE / 2,
                          10);

  GT_EXPECT_EQ(AuthenticateFirmware(test_fw.data(), &pub_key_manifest),
               FW_AUTH_EXTRA_DATA_AFTER_L1);
}

DEF_TEST(FwAuthenticateFirmware, L1OffsetInL0) {
  // Make the L0 header point at another point inside L0

  test_fw.SetImageSizes(L0_IMAGE_SIZE / 2, L1_IMAGE_SIZE / 2);
  test_fw.SetL1Offset(100);
  test_fw.Generate();

  GT_EXPECT_EQ(AuthenticateFirmware(test_fw.data(), &pub_key_manifest),
               FW_AUTH_INVALID_L1_OFFSET);
}

DEF_TEST(FwAuthenticateFirmware, L1OffsetRightBeforeEndL0) {
  // Make the L0 header point right before the end of L0

  test_fw.SetImageSizes(L0_IMAGE_SIZE / 2, L1_IMAGE_SIZE / 2);
  test_fw.SetL1Offset(IMAGE_HEADER_SIZE + L0_IMAGE_SIZE / 2 - 1);
  test_fw.Generate();

  GT_EXPECT_EQ(AuthenticateFirmware(test_fw.data(), &pub_key_manifest),
               FW_AUTH_INVALID_L1_OFFSET);
}

DEF_TEST(FwAuthenticateFirmware, L1OffsetRightAfterEndL0) {
  // Make the L0 header point right after the end of L0

  test_fw.SetImageSizes(L0_IMAGE_SIZE / 2, L1_IMAGE_SIZE / 2);
  test_fw.SetL1Offset(IMAGE_HEADER_SIZE + L0_IMAGE_SIZE / 2);
  test_fw.Generate();

  GT_EXPECT_EQ(AuthenticateFirmware(test_fw.data(), &pub_key_manifest),
               FW_AUTH_INVALID_L1);
}

DEF_TEST(FwAuthenticateFirmware, L1OffsetWrong) {
  // Make the L0 header point at a valid location in the FW slot, but that
  // doesn't contain a valid L1 header

  test_fw.SetImageSizes(L0_IMAGE_SIZE / 2, L1_IMAGE_SIZE / 2);
  test_fw.SetL1Offset(L0_TOTAL_SIZE + 10);
  test_fw.Generate();

  GT_EXPECT_EQ(AuthenticateFirmware(test_fw.data(), &pub_key_manifest),
               FW_AUTH_INVALID_L1);
}

DEF_TEST(FwAuthenticateFirmware, L1OffsetRightBeforeEndOfSpace) {
  // Make the L0 header point right before the end of the FW slot

  test_fw.SetImageSizes(L0_IMAGE_SIZE / 2, L1_IMAGE_SIZE / 2);
  test_fw.SetL1Offset(FW_SLOT_SIZE - IMAGE_HEADER_SIZE);
  test_fw.Generate();

  GT_EXPECT_EQ(AuthenticateFirmware(test_fw.data(), &pub_key_manifest),
               FW_AUTH_INVALID_L1);
}

DEF_TEST(FwAuthenticateFirmware, L1OffsetRightAfterEndOfSpace) {
  // Make the L0 header point right after the end of the FW slot

  test_fw.SetImageSizes(L0_IMAGE_SIZE / 2, L1_IMAGE_SIZE / 2);
  test_fw.SetL1Offset(FW_SLOT_SIZE - IMAGE_HEADER_SIZE + 1);
  test_fw.Generate();

  GT_EXPECT_EQ(AuthenticateFirmware(test_fw.data(), &pub_key_manifest),
               FW_AUTH_INVALID_L1_OFFSET);
}

DEF_TEST(FwAuthenticateFirmware, L1OffsetOutsideBounds) {
  // Make the L0 header point at a point outside of the FW slot

  test_fw.SetImageSizes(L0_IMAGE_SIZE / 2, L1_IMAGE_SIZE / 2);
  test_fw.SetL1Offset(L1_TOTAL_SIZE * 100);
  test_fw.Generate();

  GT_EXPECT_EQ(AuthenticateFirmware(test_fw.data(), &pub_key_manifest),
               FW_AUTH_INVALID_L1_OFFSET);
}
} // namespace FwAuthenticateFirmware

namespace FwUDS {
DEF_TEST(FwUDS, Initialize) {
  // Make sure that the UDS is initialized at first boot
  ClearMemory();
  ClearUDS();

  memcpy(GetFlashStart(), complete_fw.data(), complete_fw.length());

  hw_uds reference = {0};
  int ret = DeviceSecretGet(&reference);
  GT_EXPECT_EQ(0, ret);

  GT_EXPECT_EQ(DICE_main_host(), 0);

  hw_uds UDS = {0};
  ret = DeviceSecretGet(&UDS);
  GT_EXPECT_EQ(0, ret);

  GT_EXPECT_NE(0, memcmp(&UDS, &reference, sizeof(hw_uds)));
}

DEF_TEST(FwUDS, Persist) {
  // Make sure that the UDS persists after a reboot
  ClearMemory();
  ClearUDS();

  hw_uds UDS_first = {0};
  hw_uds UDS_second = {0};

  memcpy(GetFlashStart(), complete_fw.data(), complete_fw.length());

  // Run DICE for the first time, this sets the UDS
  GT_EXPECT_EQ(DICE_main_host(), 0);

  int ret = DeviceSecretGet(&UDS_first);
  GT_EXPECT_EQ(0, ret);

  // This second call shouldn't refresh the UDS
  GT_EXPECT_EQ(DICE_main_host(), 0);

  ret = DeviceSecretGet(&UDS_second);
  GT_EXPECT_EQ(0, ret);

  // Make sure the UDS is the same
  GT_EXPECT_EQ(0, memcmp(&UDS_first, &UDS_second, sizeof(hw_uds)));
}
} // namespace FwUDS

namespace FwBoot {
DEF_TEST(FwBoot, Boot) {
  // Normal FW boot
  ClearMemory();
  ClearUDS();

  memcpy(GetFlashStart(), complete_fw.data(), complete_fw.length());

  GT_EXPECT_EQ(DICE_main_host(), 0);
  GT_EXPECT_EQ(L0_main_host(), 0);
  GT_EXPECT_EQ(L1_main_host(), 0);
}

DEF_TEST(FwBoot, BootWatchdogReset) {
  // Reboot caused by watchdog

  ClearMemory();
  ClearUDS();

  memcpy(GetFlashStart(), complete_fw.data(), complete_fw.length());

  ResetReason_Set(RESET_REASON_WATCHDOG);

  GT_EXPECT_EQ(DICE_main_host(), -1);
}

DEF_TEST(FwBoot, FailAuthenticateL0) {
  // Boot with a corrupted L0 image
  ClearMemory();
  ClearUDS();

  // Copy DICE, the header of L0 and a bit more. This should make DICE fail
  // to authenticate L0.
  memcpy(GetFlashStart(), complete_fw.data(),
         DICE_TOTAL_SIZE + IMAGE_HEADER_SIZE + 1024);

  // DICE authenticates both L0 and L1, so it should fail
  GT_EXPECT_NE(DICE_main_host(), 0);
}
} // namespace FwBoot

int main(int argc, char **argv) {
  if (argv == nullptr || argv[1] == nullptr)
    return -1;

  ::testing::InitGoogleTest(&argc, argv);

  std::string keyPathManifest = "";
  std::string keyPathL0 = "";
  std::string keyPathL1 = "";

  try {
    cxxopts::Options options(argv[0], "FW tests");

    options.positional_help("MANIFEST_KEY_PATH L0_KEY_PATH L1_KEY_PATH");

    options.add_options()("key_manifest", "Manifest private key path",
                          cxxopts::value<std::string>(keyPathManifest))(
        "key_l0", "L0 private key path",
        cxxopts::value<std::string>(keyPathL0))(
        "key_l1", "L1 private key path",
        cxxopts::value<std::string>(keyPathL1))("h,help", "Print help");

    options.parse_positional({"key_manifest", "key_l0", "key_l1"});

    auto result = options.parse(argc, argv);

    if (result.count("help")) {
      LOG_INFO_STR(options.help().c_str());
      return 0;
    }

    if (!result.count("key_manifest")) {
      LOG_FAIL_FMT("Manifest private key not provided");
      return -1;
    }

    if (!result.count("key_l0")) {
      LOG_FAIL_FMT("L0 private key not provided");
      return -1;
    }

    if (!result.count("key_l1")) {
      LOG_FAIL_FMT("L1 private key not provided");
      return -1;
    }
  } catch (const cxxopts::OptionException &e) {
    LOG_FAIL_FMT("Error parsing options: %s", e.what());
    return 1;
  }

  // Make test reproducible
  RNG_Seed(0x1234, 0x5678);

  // Load private keys
  if (crypto_load_priv_key(keyPathManifest.c_str(), &priv_key_manifest)) {
    printf("Failed to open: %s", keyPathManifest.c_str());
    return -1;
  }

  if (crypto_load_priv_key(keyPathL0.c_str(), &priv_key_l0)) {
    printf("Failed to open: %s", keyPathL0.c_str());
    return -1;
  }

  if (crypto_load_priv_key(keyPathL1.c_str(), &priv_key_l1)) {
    printf("Failed to open: %s", keyPathL1.c_str());
    return -1;
  }

  // Calculate public keys
  if (crypto_gen_pub_key(&pub_key_manifest, &priv_key_manifest) != 0) {
    printf("Failed to calculate manifest public key");
    return -1;
  }

  if (crypto_gen_pub_key(&pub_key_l0, &priv_key_l0) != 0) {
    printf("Failed to calculate L0 public key");
    return -1;
  }

  if (crypto_gen_pub_key(&pub_key_l1, &priv_key_l1) != 0) {
    printf("Failed to calculate L1 public key");
    return -1;
  }

  // Generate key manifest
  if (keyManifest.Generate() != 0) {
    printf("Failed to generate key manifest");
    return -1;
  }

  // Generate test images
  if (complete_fw.Generate(L0_IMAGE_SIZE, L1_IMAGE_SIZE) != 0) {
    printf("Failed to generate complete image");
    return -1;
  }

  if (update_fw.Generate(L0_IMAGE_SIZE, L1_IMAGE_SIZE) != 0) {
    printf("Failed to generate update image");
    return -1;
  }

  return RUN_ALL_TESTS();
}
