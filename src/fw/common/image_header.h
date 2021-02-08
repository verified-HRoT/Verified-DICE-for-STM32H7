/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

#include <assert.h>
#include <stdint.h>

#include "common/machine.h"
#include "crypto/ImageSigning.h"
#include "crypto/crypto.h"

EXTERNAL_C_BEGIN

// Key Manifest
// ------------

// Described in cert_spec. Chapter 4.1.1.3

#define KEY_MANIFEST_VERSION (0)

#pragma pack(push, 1)
typedef struct {
  uint32_t version;       // Version of the manifest
  fw_pub_key l0_pub_key;  // Public key of L0
  fw_pub_key l1_pub_key;  // Public key of L1
  fw_signature signature; // Signature of manifest (excluding itself)
} key_manifest_t;
#pragma pack(pop)

static_assert(sizeof(key_manifest_t) == (4 + 32 + 32 + 64),
              "Incorrect manifest size");

// Firmware Image Header
// ---------------------

// Described in cert_spec. Chapter 2.3

#define IMAGE_HEADER_MAGIC (0x57464343) // 'CCFW'

#define IMAGE_HEADER_SIZE (1024) // 1 KB

#define IMAGE_HEADER_PADDING 912

#pragma pack(push, 1)
typedef struct {
  uint32_t magic;             // Magic string 'CCFW' // 4 bytes
  uint32_t fw_version;        // Firmware version // 4 bytes
  uint32_t fw_size;           // Firmware size (bytes) // 4 bytes
  digest fw_hash;             // SHA-256 of the firmware image // 32 bytes
  uint32_t next_image_offset; // Relative location of next image (0 if none) //
                              // 4 bytes
  uint8_t padding[IMAGE_HEADER_PADDING]; // Must be zero
  fw_signature
      header_signature; // Signature of header (excluding itself) // 64 bytes
} image_header_t;
#pragma pack(pop)

static_assert(sizeof(image_header_t) ==
                  (4 + 4 + 4 + 32 + 4 + IMAGE_HEADER_PADDING + 64),
              "Incorrect manifest size");

static_assert(sizeof(image_header_t) == IMAGE_HEADER_SIZE,
              "Incorrect header size");

// Interrupt vectors aligned to 1KB after the image header
#define FIRMWARE_IMAGE_OFFSET IMAGE_HEADER_SIZE

EXTERNAL_C_END
