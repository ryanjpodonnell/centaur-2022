#include <Arduino.h>
#include "Bonus.h"
#include "SharedVariables.h"

void AddToBonus(byte amountToAdd) {
  CurrentBonus += amountToAdd;
  if (CurrentBonus >= MAX_DISPLAY_BONUS) {
    CurrentBonus = MAX_DISPLAY_BONUS;
  }
}

void IncreaseBonusX() {
  boolean soundPlayed = false;
  if (BonusX[CurrentPlayer] < 5) {
    BonusX[CurrentPlayer] += 1;
    BonusXAnimationStart = CurrentTime;

    if (BonusX[CurrentPlayer] == 4) {
      BonusX[CurrentPlayer] += 1;
    }
  }
}
