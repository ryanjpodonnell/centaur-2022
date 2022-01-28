#include "SharedVariables.h"

DisplayHelper::DisplayHelper() {
  lastFlashOrDash_                              = 0;
  lastScrollPhase_                              = 0;
  lastTimeOverrideAnimated_                     = 0;
  lastTimeScoreChanged_                         = 0;
  unsigned long scoreOverrideValue_[]           = { 0, 0, 0, 0 };
  unsigned long scoreOverrideStatus_[]          = { false, false, false, false };
  unsigned long scoreOverrideAnimationStatus_[] = { false, false, false, false };
}

byte DisplayHelper::getDisplayMask(byte numDigits) {
  byte displayMask = 0;
  for (byte digitCount = 0; digitCount < numDigits; digitCount++) {
    displayMask |= (0x40 >> digitCount);
  }
  return displayMask;
}

byte DisplayHelper::magnitudeOfScore(unsigned long score) {
  if (score == 0) return 0;

  byte magnitudeOfScore = 0;
  while (score > 0) {
    score = score / 10;
    magnitudeOfScore += 1;
  }

  return magnitudeOfScore;
}

byte DisplayHelper::numberOfDigits(unsigned long score) {
  byte numDigits = magnitudeOfScore(score);
  if (numDigits == 0) return 1;

  return numDigits;
}

void DisplayHelper::overrideScoreDisplay(byte displayNum, unsigned long score, boolean animate) {
  scoreOverrideStatus_[displayNum] = true;
  if (animate) scoreOverrideStatus_[displayNum] = true;

  scoreOverrideValue_[displayNum] = score;
}

void DisplayHelper::startScoreAnimation(unsigned long scoreToAnimate) {
  if (ScoreAdditionAnimation != 0) {
    g_machineState.increaseScore(ScoreAdditionAnimation);
  }
  unsigned long currentTime = g_machineState.currentTime();
  ScoreAdditionAnimation = scoreToAnimate;
  ScoreAdditionAnimationStartTime = currentTime;
  LastRemainingAnimatedScoreShown = 0;
}

void DisplayHelper::showAnimatedPlayerScore(byte playerNumber, unsigned long score) {
  if (overrideAnimationSeed_ != lastTimeOverrideAnimated_) {
    byte numDigits = numberOfDigits(score);
    updateLastTimeAnimated_ = true;

    byte shiftDigits = (overrideAnimationSeed_) % (((BALLY_STERN_OS_NUM_DIGITS + 1) - numDigits) + ((BALLY_STERN_OS_NUM_DIGITS - 1) - numDigits));
    if (shiftDigits >= ((BALLY_STERN_OS_NUM_DIGITS + 1) - numDigits)) shiftDigits = (BALLY_STERN_OS_NUM_DIGITS - numDigits) * 2 - shiftDigits;
    byte digitCount;
    byte displayMask = getDisplayMask(numDigits);
    for (digitCount = 0; digitCount < shiftDigits; digitCount++) {
      score *= 10;
      displayMask = displayMask >> 1;
    }
    BSOS_SetDisplayBlank(playerNumber, 0x00);
    BSOS_SetDisplay(playerNumber, score, false);
    BSOS_SetDisplayBlank(playerNumber, displayMask);
  }
}

void DisplayHelper::scrollScore(byte playerNumber, unsigned long score) {
  unsigned long currentTime              = g_machineState.currentTime();
  unsigned long timeSinceLastScoreChange = currentTime - lastTimeScoreChanged_;
  byte displayMask                       = 0x7F;
  byte scrollPhaseChanged                = false;

  byte scrollPhase = (timeSinceLastScoreChange / 250) % 16;
  if (scrollPhase != lastScrollPhase_) {
    lastScrollPhase_ = scrollPhase;
    scrollPhaseChanged = true;
  }

  if ((currentTime - lastTimeScoreChanged_) < 4000) {
    BSOS_SetDisplay(playerNumber, score % (BALLY_STERN_OS_MAX_DISPLAY_SCORE + 1), false);
    BSOS_SetDisplayBlank(playerNumber, BALLY_STERN_OS_ALL_DIGITS_MASK);
  } else {
    // Scores are scrolled 10 digits and then we wait for 6
    if (scrollPhase < 11 && scrollPhaseChanged) {
      byte numDigits = magnitudeOfScore(score);

      // Figure out top part of score
      unsigned long tempScore = score;
      if (scrollPhase < BALLY_STERN_OS_NUM_DIGITS) {
        displayMask = BALLY_STERN_OS_ALL_DIGITS_MASK;
        for (byte scrollCount = 0; scrollCount < scrollPhase; scrollCount++) {
          score = (score % (BALLY_STERN_OS_MAX_DISPLAY_SCORE + 1)) * 10;
          displayMask = displayMask >> 1;
        }
      } else {
        score = 0;
        displayMask = 0x00;
      }

      // Add in lower part of score
      if ((numDigits + scrollPhase) > 10) {
        byte numDigitsNeeded = (numDigits + scrollPhase) - 10;
        for (byte scrollCount = 0; scrollCount < (numDigits - numDigitsNeeded); scrollCount++) {
          tempScore /= 10;
        }
        displayMask |= getDisplayMask(magnitudeOfScore(tempScore));
        score += tempScore;
      }
      BSOS_SetDisplayBlank(playerNumber, displayMask);
      BSOS_SetDisplay(playerNumber, score);
    }
  }
}

void DisplayHelper::flashScore(byte playerNumber, unsigned long score) {
  unsigned long currentTime = g_machineState.currentTime();
  unsigned long flashSeed = currentTime / 250;

  if (flashSeed != lastFlashOrDash_) {
    lastFlashOrDash_ = flashSeed;

    if (((currentTime / 250) % 2) == 0) BSOS_SetDisplayBlank(playerNumber, 0x00);
    else BSOS_SetDisplay(playerNumber, score, true, 2);
  }
}

void DisplayHelper::dashScore(byte playerNumber, unsigned long score) {
  unsigned long currentTime = g_machineState.currentTime();
  unsigned long dashSeed = currentTime / 50;
  byte          displayMask = 0x7F;

  if (dashSeed != lastFlashOrDash_) {
    lastFlashOrDash_ = dashSeed;
    byte dashPhase = (currentTime / 60) % 36;
    byte numDigits = magnitudeOfScore(score);

    if (dashPhase < 12) {
      displayMask = getDisplayMask((numDigits == 0) ? 2 : numDigits);
      if (dashPhase < 7) {
        for (byte maskCount = 0; maskCount < dashPhase; maskCount++) {
          displayMask &= ~(0x01 << maskCount);
        }
      } else {
        for (byte maskCount = 12; maskCount > dashPhase; maskCount--) {
          displayMask &= ~(0x20 >> (maskCount - dashPhase - 1));
        }
      }
      BSOS_SetDisplay(playerNumber, score);
      BSOS_SetDisplayBlank(playerNumber, displayMask);
    } else {
      BSOS_SetDisplay(playerNumber, score, true, 2);
    }
  }
}

void DisplayHelper::showPlayerScores(byte playerNumber, boolean flashCurrent, boolean dashCurrent, unsigned long allScoresShowValue) {
  if (playerNumber == 0xFF) unsigned long scoreOverrideStatus_[] = { false, false, false, false };
  unsigned long playerScore = 0;
  unsigned long currentTime = g_machineState.currentTime();

  overrideAnimationSeed_  = currentTime / 250;
  updateLastTimeAnimated_ = false;


  for (byte playerIterator = 0; playerIterator < 4; playerIterator++) {
    if (allScoresShowValue == 0 && scoreOverrideStatus_[playerIterator]) {
      playerScore = scoreOverrideValue_[playerIterator];
      if (playerScore == DISPLAY_OVERRIDE_BLANK_SCORE) {
        BSOS_SetDisplayBlank(playerIterator, 0);
      } else {
        byte numDigits = numberOfDigits(playerScore);
        if (numDigits < (BALLY_STERN_OS_NUM_DIGITS - 1) && scoreOverrideAnimationStatus_[playerIterator]) {
          showAnimatedPlayerScore(playerIterator, playerScore);
        } else {
          BSOS_SetDisplay(playerIterator, playerScore, true, 1);
        }
      }
    } else {
      // No override, update scores designated by playerNumber
      if (allScoresShowValue == 0) playerScore = g_machineState.score(playerIterator);
      else playerScore = allScoresShowValue;

      // If we're updating all displays, or the one currently matching the loop, or if we have to scroll
      if (playerNumber == 0xFF || playerNumber == playerIterator || playerScore > BALLY_STERN_OS_MAX_DISPLAY_SCORE) {

        // Don't show this score if it's not a current player score (even if it's scrollable)
        if (playerNumber == 0xFF && (playerIterator >= g_machineState.numberOfPlayers() && g_machineState.numberOfPlayers() != 0) && allScoresShowValue == 0) {
          BSOS_SetDisplayBlank(playerIterator, 0x00);
          continue;
        }

        if (playerScore > BALLY_STERN_OS_MAX_DISPLAY_SCORE) {
          scrollScore(playerIterator, playerScore);
        } else {
          if (flashCurrent) {
            flashScore(playerIterator, playerScore);
          } else if (dashCurrent) {
          } else {
            BSOS_SetDisplay(playerIterator, playerScore, true, 2);
          }
        }
      } // End if this display should be updated
    } // End on non-overridden
  } // End loop on scores

  if (updateLastTimeAnimated_) {
    lastTimeOverrideAnimated_ = overrideAnimationSeed_;
  }
}
