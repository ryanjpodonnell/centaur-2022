#ifndef SELF_TEST_H

#define MACHINE_STATE_ATTRACT             0
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

int RunBaseSelfTest(int curState, boolean curStateChanged, unsigned long CurrentTime, byte resetSwitch, byte slamSwitch=0xFF);
int RunSelfTest(int curState, boolean curStateChanged);
unsigned long GetAwardScore(byte level);
unsigned long GetLastSelfTestChangedTime();
void SetLastSelfTestChangedTime(unsigned long setSelfTestChange);

#endif
