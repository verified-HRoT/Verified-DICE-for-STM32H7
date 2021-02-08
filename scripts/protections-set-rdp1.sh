#!/bin/bash

programmerpath="/mnt/c/Program Files/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin"
programmertool="$programmerpath/STM32_Programmer_CLI.exe"

# Set option bytes and RDP level to 1. The protections must be clear before
# -------------------------------------------------------------------------

# Reset everything, enable WRP protection

"$programmertool" \
    -c port=SWD mode=UR reset=HWrst \
    -ob \
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
    nWRP8=0 nWRP9=1 nWRP10=1 nWRP11=1 nWRP12=1 nWRP13=1 nWRP14=1 nWRP15=1 \
    \
    SECURITY=1 \
    SEC_AREA_START1=0xFFF SEC_AREA_END1=0x0 DMES1=0 \
    SEC_AREA_START2=0xFFF SEC_AREA_END2=0x0 DMES2=0 \

ret=$?
if [ $ret != 0 ]; then
  exit 1
fi

#echo "Disconnect the Nucleo board from the USB and reconnect it."
#
#read -p "Press enter to continue"
#
## Set Secure Area
#
#"$programmertool" \
#    -c port=SWD mode=HotPlug \
#    -ob SEC_AREA_START1=0x0 SEC_AREA_END1=0x203
#
#ret=$?
#if [ $ret != 0 ]; then
#  exit 1
#fi

# Set RDP level

echo "Disconnect the Nucleo board from the USB and reconnect it."

read -p "Press enter to continue"

"$programmertool" \
    -c port=SWD mode=HotPlug \
    -ob RDP=0xBB

ret=$?
if [ $ret != 0 ]; then
  exit 1
fi

# Reset so that the RDP level change has effect

echo "Disconnect the Nucleo board from the USB and reconnect it."

read -p "Press enter to continue"

# Done

exit 0
