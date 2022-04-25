#include "SharedVariables.h"

SoundHelper::SoundHelper() {
}

void SoundHelper::playSound(byte soundEffectNum) {
  if (DEBUG_MESSAGES) {
    char buf[129];
    sprintf(buf, "Sound # %d\n", soundEffectNum);
    Serial.write(buf);
  }

  BSOS_PlaySoundSquawkAndTalk(soundEffectNum);
}

void SoundHelper::stopAudio() {
  BSOS_PlaySoundSquawkAndTalk(5);
}
