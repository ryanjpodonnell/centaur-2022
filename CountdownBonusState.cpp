#include "SharedVariables.h"

int CountdownBonus(boolean curStateChanged) {
  unsigned long currentTime = g_machineState.currentTime();
  byte currentBonus = g_machineState.bonus();

  if (curStateChanged) {
    g_machineState.setBonus(currentBonus);

    CountdownStartTime = currentTime;
    LastCountdownReportTime = CountdownStartTime;
    BonusCountDownEndTime = 0xFFFFFFFF;
  }

  unsigned long countdownDelayTime = 250 - (currentBonus * 3);
  if ((currentTime - LastCountdownReportTime) > countdownDelayTime) {
    if (currentBonus) {
      if (NumTiltWarnings <= MaxTiltWarnings) {
        unsigned long bonusMultiplier = (unsigned long)g_machineState.bonusMultiplier();
        unsigned long bonusValue = bonusMultiplier * 1000;
        g_machineState.increaseScore(bonusValue);
      }

      g_machineState.decreaseBonus(1);

    } else if (BonusCountDownEndTime == 0xFFFFFFFF) {
      BonusCountDownEndTime = currentTime + 1000;
    }

    LastCountdownReportTime = currentTime;
  }

  if (currentTime > BonusCountDownEndTime) {
    BonusCountDownEndTime = 0xFFFFFFFF;
    return MACHINE_STATE_BALL_OVER;
  }

  return MACHINE_STATE_COUNTDOWN_BONUS;
}
