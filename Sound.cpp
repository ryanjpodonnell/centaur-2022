#include "SharedVariables.h"

SoundHelper::SoundHelper() {
  uninterruptableSoundPlayingUntil_ = 0;
}

void SoundHelper::playSound(byte soundEffectNum) {
  if (g_machineState.currentTime() > uninterruptableSoundPlayingUntil_) {
    playSquawkAndTalk(soundEffectNum);
  }
}

void SoundHelper::playSoundWithoutInterruptions(byte soundEffectNum) {
  unsigned long soundEffectLength = soundDurations_[soundEffectNum] * MILLISECONDS_PER_UNIT;
  uninterruptableSoundPlayingUntil_ = g_machineState.currentTime() + soundEffectLength;

  playSquawkAndTalk(soundEffectNum);
}

void SoundHelper::stopAudio() {
  BSOS_PlaySoundSquawkAndTalk(SOUND_MUTE);
}

/*********************************************************************
    Private
*********************************************************************/
void SoundHelper::playSquawkAndTalk(byte soundEffectNum) {
  if (DEBUG_MESSAGES) {
    char buf[129];
    sprintf(buf, "Sound # %d\n", soundEffectNum);
    Serial.write(buf);
  }

  BSOS_PlaySoundSquawkAndTalk(soundEffectNum);
}
