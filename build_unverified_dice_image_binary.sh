#! /bin/bash

read -p "[Experiment] Press Enter to build the unverified dice image binary"

./build.sh -n

UNVERIFIED_DICE_DIR=./experiment/firmware/unverified_dice

mkdir -p ${UNVERIFIED_DICE_DIR}

cp ./build/arm/src/ccu_fw/L0_Image/arm/L0_Image.bin ${UNVERIFIED_DICE_DIR}/DICE_Engine.bin
cp ./build/arm/src/ccu_fw/L1_Image/arm/L1_Image.bin ${UNVERIFIED_DICE_DIR}/DICE_L0.bin

printf "\n[Experiment] the built binary of verified DICE Engine is %s\n" ${UNVERIFIED_DICE_DIR}/DICE_Engine.bin
printf "[Experiment] the built binary of verified DICE L0 is %s\n" ${UNVERIFIED_DICE_DIR}/DICE_L0.bin

printf "[Experiment] Size of the binaries:\n"
du -h ${UNVERIFIED_DICE_DIR}/*

# read -p "[Experiment] Press Enter to measure the binary size of unverified DICE"

# ./scripts/table_2_step_2_measure_binary_size.sh
