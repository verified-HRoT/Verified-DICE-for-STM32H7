#!/bin/bash

programmerpath="/mnt/c/Program Files/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin"
programmertool="$programmerpath/STM32_Programmer_CLI.exe"

# Clear option bytes and clear memory by going from RDP level 1 to 0
#
# The following code only works if the SECURITY bit is 0 and there are no secure
# areas. If not, an "Under Reset" connection doesn't work, it is needed to use
# the hotplug mode. If not using the SECURITY bit, the following works as well:
#
#     -c port=SWD mode=UR reset=HWrst

"$programmertool" \
    -c port=SWD mode=HotPlug \
    -ob \
    \
    RDP=0xAA \
    \
    RSS1=0 \
    \
    BOR_LEV=0 \
    \
    IWDG1_SW=1 NRST_STOP_D1=1 NRST_STBY_D1=1 FZ_IWDG_STOP=1 FZ_IWDG_SDBY=1 \
    BCM7=1 NRST_STOP_D2=1 NRST_STBY_D2=1 SWAP_BANK=0 \
    \
    BOOT_CM7_ADD0=0x800 BOOT_CM7_ADD1=0x800 \
    \
    PROT_AREA_START1=0xFF PROT_AREA_END1=0x0 DMEP1=0 \
    PROT_AREA_START2=0xFF PROT_AREA_END2=0x0 DMEP2=0 \
    \
    ST_RAM_SIZE=0x2 \
    \
    nWRP0=1 nWRP1=1 nWRP2=1 nWRP3=1 nWRP4=1 nWRP5=1 nWRP6=1 nWRP7=1 \
    nWRP8=1 nWRP9=1 nWRP10=1 nWRP11=1 nWRP12=1 nWRP13=1 nWRP14=1 nWRP15=1 \
    \
    SECURITY=0 \
    SEC_AREA_START1=0xFFF SEC_AREA_END1=0x0 DMES1=0 \
    SEC_AREA_START2=0xFFF SEC_AREA_END2=0x0 DMES2=0 \

ret=$?
if [ $ret != 0 ]; then
  exit 1
fi
exit 0
