/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

namespace Logger {
class ILowLevelOutput {
public:
  virtual void output(const char *str) = 0;
};

// global
ILowLevelOutput *getOutput();

void installOutput(ILowLevelOutput *target);
} // namespace Logger