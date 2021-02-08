#!/bin/bash

programmerpath="/mnt/c/Program Files/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin"
programmertool="$programmerpath/STM32_Programmer_CLI.exe"

# Clear flag changes by DICE and clear memory by going from RDP level 1 to 0

./protections-clear.sh

ret=$?
if [ $ret != 0 ]; then
  exit 1
fi

# Flash binary file, verify, reset and boot

"$programmertool" \
    -c port=SWD mode=UR reset=HWrst \
    -d ../build/arm/src/fw/arm/DICE_L0_L1.bin 0x08000000 \
    -v \
    -hardRst --start

ret=$?
if [ $ret != 0 ]; then
  exit 1
fi

# Done

exit 0
