#include "SharedVariables.h"

HertzLogging::HertzLogging() {}

void HertzLogging::run() {
  unsigned long currentTime = g_machineState.currentTime();

  numLoops_ += 1;
  if (currentTime > (lastLoopReportTime_+1000)) {
    lastLoopReportTime_ = currentTime;

    char buf[128];
    sprintf(buf, "Loop running at %lu Hz\n", numLoops_);
    Serial.write(buf);

    numLoops_ = 0;
  }
}
