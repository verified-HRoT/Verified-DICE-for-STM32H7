/*
 * Copyright(c) 2020 Microsoft. All rights reserved.
 */

// This tool generates an UNSIGNED key manifest with the public keys of L0 and
// L1. It doesn't save the signature field, which is the last field of the
// manifest. That field must be generated with signtool.
//
// This tool takes two private keys in PEM format (L0 and L1), calculates the
// public keys associated with them, and generates the key manifest that
// contains them.
//
// To invoke it:
//
//     keymanifesttool --l0key L0_KEY_PEM.txt --l1key L1_KEY_PEM.txt --out
//     unsigned_key_manifest.bin
//

#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>

#include "common/common.h"
#include "crypto/crypto_files.h"
#include "external/cxxopts.h"
#include "external/gsl.h"
#include "fw/common/image_header.h"

namespace fs = std::filesystem;

void writeFile(const std::string &fname, const std::vector<uint8_t> &file) {
  std::ofstream oFile(fname);
  const std::ostream_iterator<uint8_t> output_iterator(oFile);
  std::copy(file.begin(), file.end(), output_iterator);
  oFile.close();
}

int generateKeyManifest(const std::string &l0KeyPath,
                        const std::string &l1KeyPath,
                        const std::string &outPath) {
  int ret = 0;
  key_manifest_t manifest = {};

  // Set version of the manifest

  manifest.version = KEY_MANIFEST_VERSION;

  // Load L0 private key, generate a L0 public key and save in manifest

  fw_priv_key l0_priv_key = {};
  fw_pub_key l0_pub_key = {};

  ret = crypto_load_priv_key(l0KeyPath.c_str(), &l0_priv_key);
  if (ret != 0) {
    printf("Failed to load L0 private key: %d\n", ret);
    return -1;
  }

  ret = crypto_gen_pub_key(&l0_pub_key, &l0_priv_key);
  if (ret != 0) {
    printf("Failed to generate L0 public key: %d\n", ret);
    return -1;
  }

  memcpy(&manifest.l0_pub_key, &l0_pub_key, sizeof(l0_pub_key));

  // Load L1 private key, generate a L1 public key and save in manifest

  fw_priv_key l1_priv_key = {};
  fw_pub_key l1_pub_key = {};

  ret = crypto_load_priv_key(l1KeyPath.c_str(), &l1_priv_key);
  if (ret != 0) {
    printf("Failed to load L0 private key: %d\n", ret);
    return -1;
  }

  ret = crypto_gen_pub_key(&l1_pub_key, &l1_priv_key);
  if (ret != 0) {
    printf("Failed to generate L0 public key: %d\n", ret);
    return -1;
  }

  memcpy(&manifest.l1_pub_key, &l1_pub_key, sizeof(l1_pub_key));

  // Save manifest without signature

  constexpr size_t size = sizeof(manifest) - sizeof(manifest.signature);

  const CMemRef memRef = MakeCMemRef(&manifest, size);
  std::vector<uint8_t> outData(memRef.begin(), memRef.end());

  writeFile(outPath, outData);

  return 0;
}

int main(int argc, char *argv[]) {
  if (argv == nullptr)
    return -1;

  std::string l0KeyPath = "";
  std::string l1KeyPath = "";
  std::string outPath = "";

  try {
    cxxopts::Options options(argv[0], "Key Manifest generator tool");

    options.add_options()("l0key", "L0 private key",
                          cxxopts::value<std::string>(l0KeyPath))(
        "l1key", "L1 private key", cxxopts::value<std::string>(l1KeyPath))(
        "out", "Output path",
        cxxopts::value<std::string>(outPath))("h,help", "Print help");

    auto result = options.parse(argc, argv);

    if (result.count("help")) {
      fprintf(stderr, "%s", options.help().c_str());
      return 0;
    }
  } catch (const cxxopts::OptionException &e) {
    fprintf(stderr, "Error parsing options: %s", e.what());
    return 1;
  }

  return generateKeyManifest(l0KeyPath, l1KeyPath, outPath);
}
