/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

    .syntax unified
    .cpu cortex-m7
    .fpu softvfp
    .thumb

    // Symbols defined in the linkerscript
    .word __DTCM_START__
    .word __DTCM_LENGTH__

// void LaunchImageTrampoline(uint32_t image_address);
//
// This function jumps to the firmware image located at the given address.

    .section .LaunchImageTrampoline, "a", %progbits
    .global LaunchImageTrampoline
LaunchImageTrampoline:
    ldr     sp, [r0]
    ldr     pc, [r0, #4]

// void LaunchImageAsm(uint32_t r0, uint32_t trampoline);
//
// This function clears all DTCM, sets all registers to zero and jumps to
// the provided trampoline with "r0" in register r0.
//
// The stack is in DTCM so we can't clear it from C code as the C runtime needs
// the stack to run.

    .section .LaunchImage, "a", %progbits
    .global LaunchImageAsm
LaunchImageAsm: // r0 = argument for trampoline, r1 = trampoline

    // Clear the DTCM in 32-bit increments because the length of the RAM
    // section is always at least a multiple of 4.
    ldr     r7, =__DTCM_START__
    ldr     r8, =__DTCM_LENGTH__
    mov     r6, #0

1:  str     r6, [r7], #4
    subs    r8, #4
    bne     1b

    // Also, clear SRAM1, SRAM2 and SRAM3
    ldr     r7, =__SRAM123_START__
    ldr     r8, =__SRAM123_LENGTH__
    mov     r6, #0

2:  str     r6, [r7], #4
    subs    r8, #4
    bne     2b

    // Clear all registers
    mov     r2, #0
    mov     r3, #0
    mov     r4, #0
    mov     r5, #0
    mov     r6, #0
    mov     r7, #0
    mov     r8, #0
    mov     r9, #0
    mov     r10, #0
    mov     r11, #0
    mov     r12, #0

    // Don't clear SP or LR because RSS needs a stack pointer

    // Jump to trampoline
    mov     pc, r1
