/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

// Automated tests. On success, the functions return 0

// Try to read DICE image, which should fail as it is protected by the Secure
// User Memory, so it is read-protected..
int Test_ReadDICEImage(void);

// Try to read the key manifest, which should fail. It is protected like DICE.
int Test_ReadKeyManifest(void);

// Read memory shared between DICE and L0, which should have been erased before
// reaching L1, so it should have been cleared at the point this test runs.
int Test_ReadDICEL0SharedMemory(void);
