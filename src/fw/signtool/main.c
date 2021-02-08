/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

// This tool takes a binary and a key and generates a binary file with the
// signature. The tools headertool and keymanifesttool don't generate signed
// binaries, they generate output binaries that are actually smaller than the
// objects they create. The part that is missing is the signature. This tool
// takes the unsigned object and generates a binary file with the signature,
// which is the data that has to be appended to complete the objects.
//
// It is used like:
//
//     signtool PRIVATE_KEY_PEM.txt unsigned_object.bin output_signature.bin
//
// Then, to complete the object:
//
//     cat unsigned_object.bin output_signature.bin > signed_object.bin

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "crypto/ImageSigning.h"
#include "crypto/crypto_files.h"

static void load_file(const char *path, void **buffer, size_t *size) {
  // Make sure that the output pointers are valid
  assert(path);
  assert(buffer);
  assert(size);

  FILE *f = fopen(path, "rb");
  if (f == NULL) {
    printf("Couldn't open %s\n", path);
    exit(EXIT_FAILURE);
  }

  fseek(f, 0, SEEK_END);
  *size = ftell(f);

  if (*size == 0) {
    printf("File is empty: %s\n", path);
    fclose(f);
    exit(EXIT_FAILURE);
  }

  rewind(f);
  *buffer = malloc(*size);

  if (*buffer == NULL) {
    printf("Couldn't allocate space for: %s\n", path);
    fclose(f);
    exit(EXIT_FAILURE);
  }

  if (fread(*buffer, *size, 1, f) != 1) {
    printf("Couldn't read %s\n", path);
    fclose(f);
    exit(EXIT_FAILURE);
  }

  fclose(f);
}

static int save_signature(const char *path, fw_signature *signature) {
  FILE *f = fopen(path, "wb");
  if (f == NULL) {
    printf("Couldn't open %s for writing\n", path);
    return -1;
  }

  if (fwrite(signature, sizeof(fw_signature), 1, f) != 1) {
    printf("Failed to write data to %s\n", path);
    fclose(f);
    return -1;
  }

  fclose(f);

  return 0;
}

static void print_usage(const char *argv0) {
  printf("Usage: %s key.txt unsigned_object.bin output_signature.bin\n", argv0);
}

int main(int argc, char *argv[]) {
  if (argc != 4) {
    printf("Missing arguments.\n");
    print_usage(argv[0]);
    exit(EXIT_FAILURE);
  }

  const char *key_path = argv[1];
  const char *input_path = argv[2];
  const char *output_path = argv[3];

  fw_priv_key priv_key;
  if (crypto_load_priv_key(key_path, &priv_key) != 0)
    exit(EXIT_FAILURE);

  void *input_file;
  size_t input_file_size;
  load_file(input_path, &input_file, &input_file_size);

  fw_signature signature;
  int ret = ED25519_Sign(&signature, input_file, input_file_size, &priv_key);
  if (ret != 0) {
    printf("ED25519_Sign(): %d\n", ret);
    free(input_file);
    exit(EXIT_FAILURE);
  }

  save_signature(output_path, &signature);

  free(input_file);

  exit(EXIT_SUCCESS);
}
