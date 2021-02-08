/*
 * Copyright(c) 2020 Microsoft. All rights reserved.
 */

#include <chrono>
#include <gsl/util>

#include "common/machine.h"
#include "common/timing.h"

TimeMsType GetTimeNow(void) {
  static TimeMsType startTime = 0;
  const TimeMsType now = gsl::narrow_cast<TimeMsType>(
      std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::steady_clock::now().time_since_epoch())
          .count());

  if (startTime == 0)
    startTime = now;
  return now - startTime;
}

void ResetWatchdogTimer(void) {
  // nothing to do.
}
