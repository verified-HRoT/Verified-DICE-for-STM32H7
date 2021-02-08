# Firmware

This program is meant to be used in **NUCLEO-H753ZI** boards, but it also works
in **NUCLEO-H743ZI** and **NUCLEO-H743ZI2** boards if the RSS protection is
disabled.

Note that it is needed to configure some build options depending on the features
that have to be enabled. The CMake build system has a few options to
enable/disable protections or to select the version of the NUCLEO boards.  The
version only affects the user LEDs and the user button of the NUCLEO boards, the
binaries are otherwise identical.

This folder contains the low level code of the firmware port to the
SoC STM32H753, which contains an Arm Cortex-M7 CPU. That is, this folder contains
code that is specific this SoC and the glue code that lets it work with the rest
of the generic code. Note that most of this code can be run in the host for
testing purposes (but it doesn't test any of the hardware protections).

The code in this directory builds the 3 firmware images (DICE, L0 and L1)
and generates the signed headers of L0 and L1. Then, it concatenates DICE, L0 and
L1 to generate the full combined image, and L0 and L1 to generate an update
image to be used during the firmware flash process.

The firmware is split into 3 different images with different purposes:

- **DICE Engine (Immutable bootloader)**
  - Verifies the hardware protection features
  - Generates the Unique Device Secret (UDS)
  - Authenticates L0
  - Runs the DICE core functions
  - Receives the HW measurement

- **L0 (Secondary bootloader)**
  - Runs the L0 core functions

- **L1 (Confidential Compute engine)**
  - Runtime HW image that communicates with the host.
  - It also has support for system commands such as:
    - Hardware reset
    - Firmware version queries


## Directory Structure

The code is organized in a way that it can be mostly tested in the host without
the need for the real device. It is organized in several folders. Some of them
have ``arm`` and ``host`` subfolders that contain code specific to the port to
Arm or to the host. The result of the build is a binary file per image in the
Arm build, and a library per image in the host build (that can be used for host
testing).

- ``Keys``: Test keys used to sign L0 and L1 (DICE isn't signed
  because it is immutable).

- ``DICE_Image``: Code specific to DICE.

- ``L0_Image``: Code specific to L0.

- ``L1_Image``: Code specific to L1.

- ``tests``: Tests that can be run in the host (not in Arm builds). 
  It include tests only for the bootloader (image authentication, etc).

- ``arm``: Scripts to generate the final combined images that can be flashed to
  the device. Also, there are targets to build the STM HAL library for Arm 
  to be used by all images.

- ``common``: Code that can be used by any of the images:
  - Image authentication helpers
  - Hardware drivers (I2C, SPI, flash, watchdog, memory map, etc).

- ``config``: Configuration files for the STM HAL and HACL*

- ``headertool``: Tool that generates unsigned image headers.

- ``keymanifesttool``: Tool that generates the key manifest tool that contains
  the public keys of L0 and L1.

- ``keytool``: Tool that exports keys from PEM format to C arrays and binary
  files.

- ``option_bytes_rdp_1.csv``: File with the values that have to be set to the
  option bytes during the SFI process. This file is consumed by the **STM32
  Trusted Package Creator** that is installed along **STM32CubeProg**. This file
  corresponds to all the options set to the right value, but RDP set to level 1.
  This still allows the chip to be erased.

- ``option_bytes_rdp_2.csv``: Same as the previous one, but this sets RDP to
  level 2. This is only meant to be used for production.

- ``signtool``: Tool that generates a signature for an arbitrary binary
  specified in the command line. For example, ``headertool`` generates an
  unsigned header from a ``L0_Image.bin`` file. Then, ``signtool`` gets the
  header and a PEM key file as input and generates a binary file with the
  signature. Then, it is needed to concatenate the header with the signature to
  form the final header. This is also used for the key manifest.


## Protections

This only applies to the Arm port.

- Secure User Memory

  DICE is protected by the Secure User Memory provided by RSS. The header of L0 is
  also protected this way.

  At boot, the CPU is in secure mode, which allows it to read and write from
  this memory region. During the jump to L0 from DICE, the security mode is
  exited, which makes this memory read and write-protected. In short, DICE and the
  header of L0 can only be read and modified by DICE.

  The Unique Device Secret is stored in this memory as well.

- Write protection (WRP)

  The SoC also supports write-protection security feature. When enabled, the 
  image layer can still be read by all other images regardles of the security state.

## Mock port of the firmware to the host

Most of the code of the firmware doesn't rely on any specific hardware to run.
The way the code is organized in this folder is:

- ``component``: Generic code for this component.
  - ``arm``: Implementation for the Arm port (real code that runs on the HW).
  - ``host``: Mock implementation to use in the host.

The code in the ``arm`` subfolders can't be tested on the host, but everything
directly in ``component`` can. The folder ``host`` has some code that allows
some level of testing on the host.

The host implementation has some limitations:

- The hardware RNG isn't simulated. Only the software RNG can be used.
- The flash write/read functions don't have as many restrictions as in Arm. Most
  of them are simulated, like the minimum word size for writes (32 bytes) and
  the bank size for erases (128 KB). It even simulates the fact that in the
  flash, any 1 in memory can be set to 0, but a 0 can't be turned into a 1
  except by erasing the whole bank. However, it doesn't simulate ECC.
- The memory used to share information between images (SRAM4) is simulated and
  it should work as in the real HW. However, the flash is simulated only so
  that we can have tests about secure boot and firmware update. That is, in Arm,
  this memory is actually used for the secure boot, update, and that's the code
  that actually runs on the machine. In the host, we aren't actually running
  that code.
- The watchdog isn't simulated, and the communications aren't simulated.

## Boot flow

All images (DICE, L0 and L1) follow the same boot flow. The names used in
the following explanation correspond to the DICE code, but it applies to the other
images as well:

- Arm port:

  The entrypoint is in ``common/arm/startup_stm32h753xx.s``, which sets up
  the data and bss sections and jumps to ``main()``, located in
  ``DICE_Image/arm/main.c``. This sets up the hardware and jumps to ``DICE_main()``,
  in ``DICE_Image/DICE_main.c``, which is the generic code of DICE that can be run
  both on Arm and host. ``DICE_main()`` can return 0 or -1. If it returns 0,
  ``main()`` will jump to L0. If not, it will be stuck in DICE recovery mode.

- Host port:

  The function ``DICE_main_host()`` in ``DICE_Image/host/main.c`` is exposed as a
  library API. Internally, it calls ``DICE_main()`` and it returns the same return
  value as ``DICE_main()``.

## Unique Device Secret

The Unique Device Secret is a 32-byte value stored in DICE. In a similar way as
the firmware update flags, the initial value of all bytes of the secret is
``0xFF`` so that it can be overwritten.

DICE holds two copies of the secret to detect if it is corrupted. For example,
during the generation of the secret, it could be possible for the HW to lose
power while writing the value to flash. There would be no way to know if the
resulting value is the expected one or not. By having two copies, both values
need to be written the same way for the secret to be considered valid. If they
don't match, DICE considers it a critical error and doesn't jump to L0.


##  Memory map

The memory regions described below are the ones used by the firmware.

### Flash bank 1 (0x08000000 - 0x080FFFFF) (1M)

The first bank of flash is used for the active firmware.

| Address    | Notes                                                        |
|------------|--------------------------------------------------------------|
| 0x08000000 | DICE image start (128 KB)                                      |
| 0x080XXXXX | Unique Device Secret (arbitrary address aligned to 32 bytes) |
| 0x0801FFFF | DICE image end                                                 |
|            |                                                              |
| 0x08020000 | Key manifest start (1 KB)                                    |
| 0x080203FF | Key manifest end                                             |
|            |                                                              |
| 0x08020400 | L0 image start (256 KB - 1 KB)                               |
| 0x0805FFFF | L0 image end                                                 |
|            |                                                              |
| 0x08060000 | L1 image start (512 KB)                                      |
| 0x080DFFFF | L1 image end                                                 |
|            |                                                              |
| 0x080E0000 | Unused memory start                                          |
| 0x080FFFFF | Unused memory end (128 KB)                                   |

The memory between 0x08020000 and 0x080DFFFF is the 'active' firmware.

The memory between 0x08000000 and 0x080203FF is inside the secure user memory
area, and can't be seen once DICE jumps to L0. This includes all of DICE and the
key manifest.

Note that the first 1 KB of the L0 and L1 images is the image header.

### Flash bank 2 (0x08100000 - 0x081FFFFF) (1M)

This bank is used as not used.

### DTCM (0x20000000 - 0x2001FFFF) (128K)

This RAM is used as work RAM for the firmware images. It is cleared on exit
of DICE and L0 (along with the general purpose registers of the CPU).

Note that this RAM can't be used as source or destination of DMA copies.

### AXI SRAM (0x24000000 - 0x2407FFFF) (512K)

Currently unused. To be used as temporary storage of manifests.

### SRAM1 (0x30000000 - 0x3001FFFF) (128K)

This RAM is currently only used as source and destination of DMA transfers.

### SRAM2, SRAM3 (0x30020000 - 0x30047FFF) (160K)

This RAM is currently used to store debug logs.

### SRAM4 (0x38000000 - 0x3800FFFF) (64K)

This RAM bank is used to share information between images:

- The first 32 KB are used by DICE to pass information to L0. L0 clears this area
  before jumping to L1.

- The second 32 KB are used by L0 to pass information to L1.

### Backup SRAM (0x38800000 - 0x38800FFF) (4K)

This bank is used to share information between images. It can be trusted to
persist across reboots. It is used to:

- Pass information from L1 to DICE.

- Store a cross-image debug log to be sent to the host by L1.
