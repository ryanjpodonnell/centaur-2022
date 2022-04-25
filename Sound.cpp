#include "SharedVariables.h"

SoundHelper::SoundHelper() {
}

void SoundHelper::PlaySound(byte soundEffectNum) {
  if (DEBUG_MESSAGES) {
    char buf[129];
    sprintf(buf, "Sound # %d\n", soundEffectNum);
    Serial.write(buf);
  }

  BSOS_PlaySoundSquawkAndTalk(soundEffectNum);
}

void SoundHelper::StopAudio() {
  BSOS_PlaySoundSquawkAndTalk(5);
}
