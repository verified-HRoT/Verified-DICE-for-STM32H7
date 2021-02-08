/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

// Initialize GPIO pins used by the ready flags (in and out)
void ReadyFlag_Init(void);

// Functions used to set/reset the out flag and get its current state. This is
// the flag that is set by this chip and read by an external one.
void ReadyFlag_Out_Set(void);
void ReadyFlag_Out_Reset(void);
int ReadyFlag_Out_IsSet(void);

// This function returns the current state of the in ready flag. This one is set
// externally and can only be read on this side.
int ReadyFlag_In_IsSet(void);
