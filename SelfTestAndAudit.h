#ifndef SELF_TEST_H
#define SELF_TEST_H

#define MACHINE_STATE_TEST_LIGHTS        -1
#define MACHINE_STATE_TEST_DISPLAYS      -2
#define MACHINE_STATE_TEST_SOLENOIDS     -3
#define MACHINE_STATE_TEST_SWITCHES      -4
#define MACHINE_STATE_TEST_SOUNDS        -5
#define MACHINE_STATE_TEST_SCORE_LEVEL_1 -6
#define MACHINE_STATE_TEST_SCORE_LEVEL_2 -7
#define MACHINE_STATE_TEST_SCORE_LEVEL_3 -8
#define MACHINE_STATE_TEST_HISCR         -9
#define MACHINE_STATE_TEST_CREDITS       -10
#define MACHINE_STATE_TEST_TOTAL_PLAYS   -11
#define MACHINE_STATE_TEST_TOTAL_REPLAYS -12
#define MACHINE_STATE_TEST_HISCR_BEAT    -13
#define MACHINE_STATE_TEST_CHUTE_2_COINS -14
#define MACHINE_STATE_TEST_CHUTE_1_COINS -15
#define MACHINE_STATE_TEST_CHUTE_3_COINS -16
#define MACHINE_STATE_TEST_DONE          -17

class SelfTestAndAudit {
  private:
    unsigned long lastSolTestTime_;
    unsigned long lastSelfTestChange_;
    unsigned long savedValue_;
    unsigned long resetHold_;
    unsigned long nextSpeedyValueChange_;
    unsigned long numSpeedyChanges_;
    unsigned long lastResetPress_;
    byte          curValue_;
    byte          curSound_;
    byte          soundPlaying_;
    boolean       solenoidCycle_;

  public:
    SelfTestAndAudit();

    int           run(int curState, boolean curStateChanged);
    int           runBase(int curState, boolean curStateChanged, byte resetSwitch, byte slamSwitch=0xFF);
    unsigned long lastSelfTestChangedTime();
    void          setLastSelfTestChangedTime(unsigned long setSelfTestChange);
};

#endif
