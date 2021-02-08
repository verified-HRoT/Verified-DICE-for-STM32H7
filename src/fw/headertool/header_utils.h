/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

#include "common/machine.h"
#include "crypto/ImageSigning.h"
#include "fw/common/image_header.h"

EXTERNAL_C_BEGIN

// Fills all fields of a header except for the signature and the next image
// information. Returns 0 on success.
int image_header_build(image_header_t *header, uint32_t version, void *fw_data,
                       uint32_t fw_size, uint32_t offset);

// Signs a header that has been filled previously. Returns 0 on success.
int image_header_sign(image_header_t *header, fw_priv_key *priv_key);

EXTERNAL_C_END
