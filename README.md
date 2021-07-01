# Verified DICE based Boot Firmware for STM32H7

This repository contains a Verified Boot implementation for the STM32H7 devices (specifically STM32H753ZI, STM32H743ZI) for the [paper](https://www.usenix.org/conference/usenixsecurity21/presentation/tao). 
The implementation contains DICE* code generated from the [dice-star](https://github.com/verified-hrot/dice-star) repository and implements the Hardware Abstraction Interface.
The cmake toolchain verifies the F* specification and extracts the generated C code using [Kremlin](https://github.com/FStarLang/kremlin/). The generated code compiles a firmware binary, which can be tested on the STM32H7 devices.

## Directory layout

- ``3rdparty``: Components provided by third parties. Includes a number of external dependancies that prove hard to fetch and use with apt install.
- ``cmake``: We build with cmake and then ninja. The core cmake files are in cmake and included by directory local CMakeLists.txt files
- ``src``: Source, by library or executable. Refer [README.md](/src/README.md)  for details.
- ``scripts``: Scripts for flashing the firmware

## Setup environment

For the device, it is needed to install [**STM32CubeProgrammer**][1], the ST USB drivers
([**STSW-LINK009**][2]) and a terminal emulator that supports the **YMODEM**
protocol, such as [**Tera Term**][3] or Putty.

Update the firmware of the NUCLEO board with [**STM32CubeProgrammer**][1] before
doing anything, and make sure that all the protections enabled by option bytes
are disabled. You may also use the script `scripts/protections-clear.sh`.

### Verification and Extraction (Optional)

The project relies on [VerifiedHardware](https://github.com/95616ARG/VerifiedHardware)'s
dependencies to be installed throught the [Everest](https://github.com/project-everest/everest) script.

## Patch kremlin code

There are some fixes required on the `3rdparty/kremlin` code to build the firmware successfully. 
Please note that the patch is also applied as a part of the `build.sh` so you may skip this step.


```
# Patch kremlin code
cd 3rdparty/kremlin
git apply -v ../../patches/kremlin_firmware_fixes.patch
cd ../..
```


## Evaluate

### Build and Flash

In order to build the firmware, we use GNU Arm toolchain
(``gcc-arm-none-eabi``) [Arm's website][5]. The project uses the CMake build system.

See [here](/src/fw/README.md) for details about the firmware.

1.  On Linux-like systems, in the root directory of the project, run:

   ```
   build.sh
   ```

   This will generate the firmware in the folder ``build/src/fw/arm``.
   ``DICE_L0_L1.bin`` has all the images combined (DICE, L0 and L1), and
   ``L0_L1.bin`` is the file that can be downloaded to the board as a firmware
   update. The dependencies for building the firmware 
   can be installed using `install_deps.sh`


   At this point it is also possible to run the host tests by running:

   ```
   ninja test
   ```

   You may also use Docker for building the firmware:

   ```
   docker build .
   ```

   copy the firmware binary and flash script from the container


2. Connect the board to the USB port. If you are using Widows, note the COM port being used via Device Manager.

3. If using NUCLEO boards, check the instructions in [**en.DM00414687.pdf**][4]
   in chapter 8 for more details on how to flash binary files. The script
   ``flash.sh`` can be used to flash the images instead of any graphical
   tool. The script also removes the firmware protections, such as lowering RDP
   level 1 to level 0 (if RDP level has been set to 2, the board is locked and
   the protections can't be removed).

   It may be needed to use other tools to flash the firmware images. In that
   case, flash the following images to the specified addresses:

   - ``DICE_L0_L1.bin`` has to be flashed to ``0x08000000``.

   Note that ``L0_L1.bin`` shouldn't be flashed on its own.

4. The terminal emulator must be set up to 115200 baud. The other settings are
   most likely correct by default, but they are here for reference:

   - Baud rate: 115200
   - Data: 8 bits
   - Parity: none
   - Stop: 1 bit
   - Flow control: none

### Build options

- ``HW_MIN_LOG_LEVEL``: Selects the debug messages compiled in the 
  firmware. Normally all messages are compiled in and they are filtered at
  runtime. This option makes the compiler remove them completely. It defaults to
  ``LOG_LEVEL_TRACE``. It can be any of the defines in ``logger.h``:
  ``LOG_LEVEL_{TRACE,SECRET,DEBUG,INFO,FAIL,FATAL}``

- ``HW_IGNORE_PROTECTION_CHECKS``: Let DICE ignore all protection checks and boot
  L0 and L1 anyway. Check [this document][/src/fw/option_bytes.md] for more
  information about the protections. During development it isn't practical to
  set the protections whenever the firmware is flashed, but it is needed to
  check them in a production build. Default: ON

- ``HW_SET_RSS_PROTECTIONS_AT_RUNTIME``: Set to ON to set up the Secure User
  Area from DICE during boot. This can be used for testing the protections of the
  HW. Normally, the SFI process would set the protections when the firmware is
  flashed, but this isn't practical during development. Default: OFF

- ``HW_ENABLE_WATCHDOG``: Enable hardware and software watchdogs. Default: ON

- ``USE_HW_RNG``: Use hardware random number generator instead of software
  generator. Production builds need to use the hardware one. Default: ON

- ``USE_NUCLEO2_BOARDS``: Build for **NUCLEO-H743ZI2** boards rather than
  **NUCLEO-H743ZI**. Default: ON

### Production firmware

For development, it is useful to have some build options set to some values that
aren't valid in production. In order to build production firmware, CMake has to
be invoked with the following options:

```
cmake .. -GNinja \
    -DCROSS_COMPILE_ARM=/path/to/gcc-arm-none-eabi-8-2019-q3-update/bin \
    -DUSE_HW_RNG=ON \
    -DHW_IGNORE_PROTECTION_CHECKS=OFF \
    -DHW_MIN_LOG_LEVEL=LOG_LEVEL_DEBUG``
```

Any value over ``DEBUG`` is a valid value for ``HW_MIN_LOG_LEVEL``. That means
that the valid value are ``LOG_LEVEL_{DEBUG,INFO,FAIL,FATAL}``. This will hide
all ``SECRET`` prints.

### Test protections

The HW relies on several hardware protections being set: RSS, WRP, etc. In
order to test them in a Nucleo board:

1. Run CMake with the following options and build the firmware:

```
cmake .. -GNinja -DCROSS_COMPILE_ARM=path/to/gcc \
                 -DHW_IGNORE_PROTECTION_CHECKS=OFF \
                 -DHW_SET_RSS_PROTECTIONS_AT_RUNTIME=ON
ninja
```

2. Flash the firmware with ``flash.sh``. The firmware will try to boot but
   it will be locked in DICE in a loop while trying to set up the Secure User
   Memory.

3. Run ``protections-set-rdp1.sh``. It will ask you to disconnect the board
   and reconnect it. When this happens, the code will set up the Secure User
   Memory (which triggers another software reset).

4. Press enter to let the script continue. This will set the RDP level to 1.
   Disconnect and reconnect the board for this change to have effect.

5. Now that all protections have been set, DICE can boot. It will generate the
   Unique Device Secret and enter L0 and eventually L1. The L1 tests should say
   "PROTECTION SET". If they say "PROTECTION NOT SET" there may have been a
   problem during the setup of the protections.

[1]: https://www.st.com/en/development-tools/stm32cubeprog.html
[2]: https://www.st.com/en/development-tools/stsw-link009.html
[3]: http://hp.vector.co.jp/authors/VA002416/teraterm.html
[4]: https://www.st.com/content/ccc/resource/technical/document/user_manual/group0/33/ee/5b/6b/c7/43/44/3e/DM00414687/files/DM00414687.pdf/jcr:content/translations/en.DM00414687.pdf
[5]: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads
