#ifndef SOUND_H
#define SOUND_H

#define MILLISECONDS_PER_UNIT                   25

#define SOUND_MUTE                              5
#define SOUND_CONTINIOUS_DRONE                  6
#define SOUND_LONG_ATTACK_1                     11
#define SOUND_LONG_ATTACK_2                     12
#define SOUND_LONG_ATTACK_3                     13
#define SOUND_LONG_ATTACK_4                     14
#define SOUND_LONG_ATTACK_5                     15
#define SOUND_DROP_TARGET                       16
#define SOUND_SLINGSHOT                         17
#define SOUND_10_POINT_REBOUND                  18
#define SOUND_ALARM                             19
#define SOUND_INLINE_BACK_TARGET                20
#define SOUND_RESET_1_THROUGH_4                 21
#define SOUND_TOP_ROLLOVER                      22
#define SOUND_BOTTOM_ROLLOVER                   23
#define SOUND_QUEENS_CHAMBER_ROLLOVER           24
#define SOUND_SPOT_1_THROUGH_4                  25
#define SOUND_5K_SWITCH                         26
#define SOUND_1K_SWITCH                         27
#define SOUND_CRASH                             28
#define SOUND_LASERS                            29
#define SOUND_DRONE_2                           30
#define SOUND_BONUS                             31
#define SOUND_CHIRP_1                           32
#define SOUND_CHIRP_2                           33
#define SOUND_CHIRP_3                           34
#define SOUND_CHIRP_4                           35
#define SOUND_CHIRP_5                           36
#define SOUND_CHIRP_6                           37
#define SOUND_SIREN_2                           38
#define SOUND_PHASER_1                          40
#define SOUND_PLACEHOLDER_6                     42
#define SOUND_POWERING_DOWN                     44
#define SOUND_2X_COMPLETED                      48
#define SOUND_3X_COMPLETED                      49
#define SOUND_4X_COMPLETED                      50
#define SOUND_5X_COMPLETED                      51
#define SOUND_COMPLETED                         52
#define SOUND_POWER_ORBS                        53
#define SOUND_BEGIN_ORBITAL_ATTACK              54
#define SOUND_ENERGIZE_ME                       55
#define SOUND_ENERGIZE_POWER_ORB                56
#define SOUND_ACTIVATE_POWER_ORB                57
#define SOUND_POWER_ORB                         58
#define SOUND_GUARDIAN_RELEASES_POWER_ORBS      59
#define SOUND_RELEASE_POWER_ORBS                60
#define SOUND_ENERGIZE                          61
#define SOUND_BAD_MOVE_HUMAN                    62
#define SOUND_SLOW_ARENT_YOU                    63
#define SOUND_SEQUENCE_ACTIVATE                 64
#define SOUND_RELEASE_TARGET_ACTIVATED          65
#define SOUND_SEQUENCE                          66
#define SOUND_ONLY_SINGLE_VALUE                 67
#define SOUND_SIDE_ORBITS_DOUBLE                68
#define SOUND_TRY                               69
#define SOUND_DOUBLE_QUEENS_CHAMBER             70
#define SOUND_NO_CLASS_HUMAN                    71
#define SOUND_CHALLENGE_ME                      72
#define SOUND_ORB_FEATURE                       73
#define SOUND_SEQUENCE_FEATURE                  74
#define SOUND_CHAMBER_FEATURE                   75
#define SOUND_GUARDIAN_FEATURE                  76
#define SOUND_BONUS_FEATURE                     77
#define SOUND_DESTROY_CENTAUR                   78
#define SOUND_HUMAN                             79
#define SOUND_CENTAUR                           80
#define SOUND_POWERLANE                         81
#define SOUND_HA_HA_HA                          82
#define SOUND_ACTIVATED                         83
#define SOUND_MAXIMUM_ORBITAL_STRENGTH_ACHIEVED 84

// 1 unit == 25 milliseconds
static byte soundDurations_[] = {
  0,   // 0
  0,   // 1
  0,   // 2
  0,   // 3
  0,   // 4
  0,   // 5
  255, // 6
  0,   // 7
  0,   // 8
  0,   // 9
  0,   // 10
  45,  // 11
  50,  // 12
  55,  // 13
  60,  // 14
  64,  // 15
  26,  // 16
  121, // 17
  30,  // 18
  129, // 19
  35,  // 20
  34,  // 21
  35,  // 22
  53,  // 23
  118, // 24
  28,  // 25
  28,  // 26
  38,  // 27
  65,  // 28
  165, // 29
  181, // 30
  72,  // 31
  20,  // 32
  22,  // 33
  21,  // 34
  23,  // 35
  22,  // 36
  23,  // 37
  255, // 38
  0,   // 39
  41,  // 40
  0,   // 41
  70,  // 42
  0,   // 43
  160, // 44
  0,   // 45
  0,   // 46
  0,   // 47
  68,  // 48
  73,  // 49
  73,  // 50
  74,  // 51
  45,  // 52
  50,  // 53
  94,  // 54
  67,  // 55
  99,  // 56
  54,  // 57
  50,  // 58
  110, // 59
  75,  // 60
  52,  // 61
  68,  // 62
  63,  // 63
  70,  // 64
  80,  // 65
  40,  // 66
  89,  // 67
  80,  // 68
  30,  // 69
  83,  // 70
  85,  // 71
  54,  // 72
  64,  // 73
  77,  // 74
  82,  // 75
  78,  // 76
  75,  // 77
  111, // 78
  31,  // 79
  66,  // 80
  43,  // 81
  47,  // 82
  38,  // 83
  128  // 84
};

class SoundHelper {
  private:
    unsigned long uninterruptableSoundPlayingUntil_;

    void playSquawkAndTalk(byte soundEffectNum);

  public:
    SoundHelper();

    void playSound(byte soundEffectNum);
    void playSoundWithoutInterruptions(byte soundEffectNum);
    void stopAudio();
};

#endif
