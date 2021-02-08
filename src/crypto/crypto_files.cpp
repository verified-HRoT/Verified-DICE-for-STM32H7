/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include <assert.h>
#include <inttypes.h>

#include "Hacl_Lib.h"
#include "common/logger.h"
#include "common/machine.h"
#include "crypto_files.h"
#include "global/crypto_defs.h"

static int load_file(const char *path, uint8_t *buffer) {
  // Make sure that the output pointers are valid
  assert(path);
  assert(buffer);

  FILE *f = fopen(path, "rb");
  if (f == NULL) {
    printf("Couldn't open %s\n", path);
    return -1;
  }

  fseek(f, 0, SEEK_END);
  size_t size = ftell(f);

  if (size == 0) {
    printf("File is empty: %s\n", path);
    fclose(f);
    return -1;
  }

  rewind(f);

  if (fread(buffer, size, 1, f) != 1) {
    printf("Couldn't read %s\n", path);
    fclose(f);
    return -1;
  }

  fclose(f);

  return 0;
}

EXTERNAL_C int crypto_load_priv_key(const char *path, fw_priv_key *priv_key) {
  assert(path);
  assert(priv_key);

  const int ret = load_file(path, priv_key->data);

  return ret;
}

EXTERNAL_C int crypto_gen_pub_key(fw_pub_key *pub_key,
                                  const fw_priv_key *priv_key) {
  assert(pub_key);
  assert(priv_key);

  Hacl_Ed25519_secret_to_public(pub_key->data, (uint8_t *)priv_key->data);

  return 0;
}
