#include <Arduino.h>
#include "BSOS_Config.h"
#include "BallySternOS.h"
#include "SharedVariables.h"
#include "Sound.h"

void PlaySound(byte soundEffectNum) {
  if (DEBUG_MESSAGES) {
    char buf[129];
    sprintf(buf, "Sound # %d\n", soundEffectNum);
    Serial.write(buf);
  }
  BSOS_PlaySoundSquawkAndTalk(soundEffectNum);
}

void StopAudio() {
  BSOS_PlaySoundSquawkAndTalk(5);
}
