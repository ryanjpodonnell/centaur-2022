#include "SharedVariables.h"

byte GetDisplayMask(byte numDigits) {
  byte displayMask = 0;
  for (byte digitCount = 0; digitCount < numDigits; digitCount++) {
    displayMask |= (0x20 >> digitCount);
  }
  return displayMask;
}

byte MagnitudeOfScore(unsigned long score) {
  if (score == 0) return 0;

  byte retval = 0;
  while (score > 0) {
    score = score / 10;
    retval += 1;
  }
  return retval;
}

void StartScoreAnimation(unsigned long scoreToAnimate) {
  if (ScoreAdditionAnimation != 0) {
    CurrentScores[CurrentPlayer] += ScoreAdditionAnimation;
  }
  ScoreAdditionAnimation = scoreToAnimate;
  ScoreAdditionAnimationStartTime = CurrentTime;
  LastRemainingAnimatedScoreShown = 0;
}

void ShowPlayerScores(byte displayToUpdate, boolean flashCurrent, boolean dashCurrent, unsigned long allScoresShowValue) {
  byte displayMask = 0x3F;
  unsigned long displayScore = 0;

  boolean updateLastTimeAnimated = false;
  for (byte scoreCount = 0; scoreCount < 4; scoreCount++) {
    if (allScoresShowValue == 0) displayScore = CurrentScores[scoreCount];
    else displayScore = allScoresShowValue;

    // If we're updating all displays, or the one currently matching the loop
    if (displayToUpdate == 0xFF || displayToUpdate == scoreCount) {

      // Don't show this score if it's not a current player score (even if it's scrollable)
      if (displayToUpdate == 0xFF && (scoreCount >= CurrentNumPlayers && CurrentNumPlayers != 0) && allScoresShowValue == 0) {
        BSOS_SetDisplayBlank(scoreCount, 0x00);
        continue;
      }

      if (flashCurrent) {
        unsigned long flashSeed = CurrentTime / 250;
        if (flashSeed != LastFlashOrDash) {
          LastFlashOrDash = flashSeed;
          if (((CurrentTime / 250) % 2) == 0) BSOS_SetDisplayBlank(scoreCount, 0x00);
          else BSOS_SetDisplay(scoreCount, displayScore, true, 2);
        }
      } else if (dashCurrent) {
        unsigned long dashSeed = CurrentTime / 50;
        if (dashSeed != LastFlashOrDash) {
          LastFlashOrDash = dashSeed;
          byte dashPhase = (CurrentTime / 60) % 36;
          byte numDigits = MagnitudeOfScore(displayScore);
          if (dashPhase < 12) {
            displayMask = GetDisplayMask((numDigits == 0) ? 2 : numDigits);
            if (dashPhase < 7) {
              for (byte maskCount = 0; maskCount < dashPhase; maskCount++) {
                displayMask &= ~(0x01 << maskCount);
              }
            } else {
              for (byte maskCount = 12; maskCount > dashPhase; maskCount--) {
                displayMask &= ~(0x20 >> (maskCount - dashPhase - 1));
              }
            }
            BSOS_SetDisplay(scoreCount, displayScore);
            BSOS_SetDisplayBlank(scoreCount, displayMask);
          } else {
            BSOS_SetDisplay(scoreCount, displayScore, true, 2);
          }
        }
      } else {
        BSOS_SetDisplay(scoreCount, displayScore, true, 2);
      }
    }
  }
}
