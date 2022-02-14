#include "SharedVariables.h"

DisplayHelper::DisplayHelper() {
  lastFlashOrDash_          = 0;
  lastScrollPhase_          = 0;
  lastTimeOverrideAnimated_ = 0;

  scoreOverrideStatus_[0] = false;
  scoreOverrideStatus_[1] = false;
  scoreOverrideStatus_[2] = false;
  scoreOverrideStatus_[3] = false;
}

void DisplayHelper::overrideScoreDisplay(byte displayNum, unsigned long score) {
  scoreOverrideStatus_[displayNum] = true;
  scoreOverrideValue_[displayNum] = score;
  showPlayerScores(displayNum);
}

// 0xFF will reset all displays to the 4 scores (blank if the player isn't active)
void DisplayHelper::showPlayerScores(byte playerNumber, boolean flashCurrent, boolean dashCurrent) {
  if (playerNumber == 0xFF) {
    scoreOverrideStatus_[0] = false;
    scoreOverrideStatus_[1] = false;
    scoreOverrideStatus_[2] = false;
    scoreOverrideStatus_[3] = false;
  }

  overrideAnimationSeed_  = g_machineState.currentTime() / 250;
  updateLastTimeAnimated_ = false;

  for (byte playerIterator = 0; playerIterator < 4; playerIterator++) {
    if (scoreOverrideStatus_[playerIterator]) showScoreOverride(playerIterator);
    else showPlayerScore(playerNumber, playerIterator, flashCurrent, dashCurrent);
  }

  if (updateLastTimeAnimated_) {
    lastTimeOverrideAnimated_ = overrideAnimationSeed_;
  }
}


/*********************************************************************
    Private
*********************************************************************/
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
  if (score == 0) return 1;

  return magnitudeOfScore(score);
}

void DisplayHelper::dashScore(byte playerNumber, unsigned long score) {
  unsigned long dashSeed = g_machineState.currentTime() / 50;
  byte          displayMask = 0x7F;

  if (dashSeed != lastFlashOrDash_) {
    lastFlashOrDash_ = dashSeed;
    byte dashPhase = (g_machineState.currentTime() / 60) % 36;
    byte numDigits = magnitudeOfScore(score);

    if (dashPhase < 12) {
      displayMask = getDisplayMask((numDigits == 0) ? 2 : numDigits);
      if (dashPhase < 7) {
        for (byte maskCount = 0; maskCount < dashPhase; maskCount++) {
          displayMask &= ~(0x01 << maskCount);
        }
      } else {
        for (byte maskCount = 12; maskCount > dashPhase; maskCount--) {
          displayMask &= ~(0x60 >> (maskCount - dashPhase - 1));
        }
      }
      BSOS_SetDisplay(playerNumber, score);
      BSOS_SetDisplayBlank(playerNumber, displayMask);
    } else {
      BSOS_SetDisplay(playerNumber, score, true, 2);
    }
  }
}

void DisplayHelper::flashScore(byte playerNumber, unsigned long score) {
  unsigned long flashSeed = g_machineState.currentTime() / 250;

  if (flashSeed != lastFlashOrDash_) {
    lastFlashOrDash_ = flashSeed;

    if (((g_machineState.currentTime() / 250) % 2) == 0) BSOS_SetDisplayBlank(playerNumber, 0x00);
    else BSOS_SetDisplay(playerNumber, score, true, 2);
  }
}

void DisplayHelper::scrollScore(byte playerNumber, unsigned long score) {
  unsigned long lastTimeScoreChanged     = g_machineState.lastScoreChangeTime();
  unsigned long timeSinceLastScoreChange = g_machineState.currentTime() - lastTimeScoreChanged;
  byte displayMask                       = 0x7F;
  byte scrollPhaseChanged                = false;

  byte scrollPhase = (timeSinceLastScoreChange / 250) % 16;
  if (scrollPhase != lastScrollPhase_) {
    lastScrollPhase_ = scrollPhase;
    scrollPhaseChanged = true;
  }

  if ((g_machineState.currentTime() - lastTimeScoreChanged) < 4000) {
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

void DisplayHelper::showPlayerScore(byte playerNumber, byte playerIterator, boolean flashCurrent, boolean dashCurrent) {
  unsigned long score = g_machineState.score(playerIterator);

  if (playerNumber == 0xFF || playerIterator == playerNumber || score > BALLY_STERN_OS_MAX_DISPLAY_SCORE) {
    if (playerNumber == 0xFF && (playerIterator >= g_machineState.numberOfPlayers() && g_machineState.numberOfPlayers() != 0)) {
      BSOS_SetDisplayBlank(playerIterator, 0x00);
      return;
    }

    if (score > BALLY_STERN_OS_MAX_DISPLAY_SCORE) {
      scrollScore(playerIterator, score);
    } else {
      if (flashCurrent) {
        flashScore(playerIterator, score);
      } else if (dashCurrent) {
        dashScore(playerIterator, score);
      } else {
        BSOS_SetDisplay(playerIterator, score, true, 2);
      }
    }
  }
}

void DisplayHelper::showScoreOverride(byte playerNumber) {
  unsigned long score = scoreOverrideValue_[playerNumber];
  if (score == DISPLAY_OVERRIDE_BLANK_SCORE) {
    BSOS_SetDisplayBlank(playerNumber, 0);
  } else {
    byte numDigits = numberOfDigits(score);
    if (numDigits < (BALLY_STERN_OS_NUM_DIGITS - 1)) {
      showAnimatedPlayerScore(playerNumber, score);
    } else {
      BSOS_SetDisplay(playerNumber, score, true, 1);
    }
  }
}
