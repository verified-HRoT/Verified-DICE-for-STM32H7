#!/bin/bash
set -e

ROOTDIR=$(realpath `pwd`)

# Patch kremlin code
cd 3rdparty/kremlin
git apply -v ../../patches/kremlin_firmware_fixes.patch
cd ../..

# Remove output directory just in case
rm -rf build

echo "---------------------- Build: ARM + Host + OpenAPI (GCC)"

mkdir -p build/arm
cd build/arm
cmake $ROOTDIR -GNinja \
    -DUSE_HW_RNG=ON \
    -DCROSS_COMPILE_ARM=$HOME/gcc-arm-none-eabi-8-2019-q3-update/bin/

ninja all
ctest -R fw_tests
ninja install
cd ../..

echo "---------------------- Build: Host + OpenAPI (GCC, Debug) + TESTS"

mkdir -p build/host
cd build/host
cmake $ROOTDIR -GNinja
# ninja also create scripts and run them before tests
ninja all SCRIPTS WHEELS
timeout 10m ctest -j `nproc` --output-on-failure
cd ../..


# Save artifacts

OUTDIR=$ROOTDIR/output
mkdir -p $OUTDIR
cp build/arm/install/firmware/* $OUTDIR
cp build/host/Testing/Temporary/LastTest.log $OUTDIR
