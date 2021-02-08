#! /bin/bash

pushd experiment &> /dev/null
./build.sh
popd &> /dev/null

VERIFIED_DICE_DIR=./experiment/firmware/verified_dice

mkdir -p ${VERIFIED_DICE_DIR}

cp ./experiment/build/arm/src/fw/DICE_Image/arm/DICE_Image.bin ${VERIFIED_DICE_DIR}/DICE_Engine.bin
cp ./experiment/build/arm/src/fw/L0_Image/arm/L0_Image.bin ${VERIFIED_DICE_DIR}/DICE_L0.bin

printf "\n[Experiment] the built binary of verified DICE Engine is %s\n" ${VERIFIED_DICE_DIR}/DICE_Engine.bin
printf "[Experiment] the built binary of verified DICE L0 is %s\n" ${VERIFIED_DICE_DIR}/DICE_L0.bin

printf "[Experiment] Size of the binaries:\n"
du -h ${VERIFIED_DICE_DIR}/*

# read -p "[Experiment] Press Enter to measure the binary size of verified DICE"
# ./scripts/table_2_step_2_measure_binary_size.sh

exit
