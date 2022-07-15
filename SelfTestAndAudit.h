#ifndef SELF_TEST_H
#define SELF_TEST_H

#define MACHINE_STATE_TEST_LIGHTS        -1
#define MACHINE_STATE_TEST_DISPLAYS      -2
#define MACHINE_STATE_TEST_SOLENOIDS     -3
#define MACHINE_STATE_TEST_SWITCHES      -4
#define MACHINE_STATE_TEST_SOUNDS        -5
#define MACHINE_STATE_TEST_FREE_PLAY     -6
#define MACHINE_STATE_TEST_HISCORE       -7
#define MACHINE_STATE_TEST_CREDITS       -8
#define MACHINE_STATE_TEST_TOTAL_PLAYS   -9
#define MACHINE_STATE_TEST_TOTAL_REPLAYS -10
#define MACHINE_STATE_TEST_HISCORE_BEAT  -11
#define MACHINE_STATE_TEST_CHUTE_2_COINS -12
#define MACHINE_STATE_TEST_CHUTE_1_COINS -13
#define MACHINE_STATE_TEST_CHUTE_3_COINS -14
#define MACHINE_STATE_TEST_DONE          -15

class SelfTestAndAudit {
  private:
    unsigned long lastResetPress_;
    unsigned long lastSelfTestChange_;
    unsigned long lastSolTestTime_;
    unsigned long nextSoundPlayTime_;
    unsigned long nextSpeedyValueChange_;
    unsigned long numSpeedyChanges_;
    unsigned long resetHold_;
    unsigned long savedValue_;
    byte          curValue_;
    byte          curSound_;
    byte          soundPlaying_;
    boolean       currentSoundChanged_;
    boolean       solenoidCycle_;

    int           runBase(int curState, boolean curStateChanged);

  public:
    SelfTestAndAudit();

    int           run(int curState, boolean curStateChanged);
};

#endif
