#define LAMP_PLAYFIELD_SPELLOUT_S         0
#define LAMP_PLAYFIELD_SPELLOUT_I1        1
#define LAMP_PLAYFIELD_SPELLOUT_L1        2
#define LAMP_PLAYFIELD_SPELLOUT_V         3
#define LAMP_PLAYFIELD_SPELLOUT_E         4
#define LAMP_PLAYFIELD_SPELLOUT_R         5
#define LAMP_PLAYFIELD_SPELLOUT_B         6
#define LAMP_PLAYFIELD_SPELLOUT_A1        7
#define LAMP_PLAYFIELD_SPELLOUT_L2        8
#define LAMP_PLAYFIELD_SPELLOUT_L3        9
#define LAMP_PLAYFIELD_SPELLOUT_M         10
#define LAMP_PLAYFIELD_SPELLOUT_A2        11
#define LAMP_PLAYFIELD_SPELLOUT_N         12
#define LAMP_PLAYFIELD_SPELLOUT_I2        13
#define LAMP_PLAYFIELD_SPELLOUT_A3        14
#define LAMP_TOPLANE_OUTER_WHEN_LIT       15
#define LAMP_PLAYFIELD_STANDUP_S          16
#define LAMP_PLAYFIELD_STANDUP_I1         17
#define LAMP_PLAYFIELD_STANDUP_L1         18
#define LAMP_PLAYFIELD_STANDUP_V          19
#define LAMP_PLAYFIELD_STANDUP_E          20
#define LAMP_PLAYFIELD_STANDUP_R          21
#define LAMP_PLAYFIELD_STANDUP_B          22
#define LAMP_PLAYFIELD_STANDUP_A1         23
#define LAMP_PLAYFIELD_STANDUP_L2         24
#define LAMP_PLAYFIELD_STANDUP_L3         25
#define LAMP_PLAYFIELD_STANDUP_M          26
#define LAMP_PLAYFIELD_STANDUP_A2         27
#define LAMP_PLAYFIELD_STANDUP_N          28
#define LAMP_PLAYFIELD_STANDUP_I2         29
#define LAMP_PLAYFIELD_STANDUP_A3         30
#define LAMP_RIGHT_SPINNER_1000_WHEN_LIT  31
#define LAMP_TOPLANE_CENTER_WHEN_LIT      32
#define LAMP_KICKER_LAMPS                 33
#define LAMP_15K_BONUS                    34
#define LAMP_30K_BONUS                    35
#define LAMP_BONUS_2X                     36
#define LAMP_BONUS_3X                     37
#define LAMP_BONUS_4X                     38
#define LAMP_KICKER_SPECIAL               39
#define LAMP_BONUS_5X                     40
#define LAMP_HEAD_MATCH                   41
#define LAMP_SHOOT_AGAIN                  42
#define LAMP_APRON_CREDIT                 43
#define LAMP_HEAD_L2                      44
#define LAMP_HEAD_L3                      45
#define LAMP_EXTRA_BALL                   46
#define LAMP_LEFT_SPINNER_1000_WHEN_LIT   47
#define LAMP_HEAD_BALL_IN_PLAY            48
#define LAMP_HEAD_HIGH_SCORE              49
#define LAMP_HEAD_GAME_OVER               50
#define LAMP_HEAD_TILT                    51
#define LAMP_HEAD_S                       52
#define LAMP_HEAD_I1                      53
#define LAMP_HEAD_L1                      54
#define LAMP_HEAD_V                       55
#define LAMP_HEAD_E                       56
#define LAMP_HEAD_R                       57
#define LAMP_HEAD_B                       58
#define LAMP_HEAD_A                       59

#define SW_4TH_BALL_TROUGH                    0
#define SW_5TH_BALL_TROUGH                    1
#define SW_TOP_RIGHT_LANE                     2
#define SW_TOP_MIDDLE_LANE                    3
#define SW_TOP_LEFT_LANE                      4
#define SW_CREDIT_BUTTON                      5
#define SW_OUTHOLE                            7
#define SW_COIN_III                           8
#define SW_COIN_I                             9
#define SW_COIN_II                            10
#define SW_TOP_LEFT_LANE_RO_BUTTON            11
#define SW_ORBS_2_BACK_TARGETS                11
#define SW_TARGET_BEHIND_RIGHT_THUMPER_BUMPER 11
#define SW_TILT                               14
#define SW_SLAM                               15
#define SW_1ST_BALL_TROUGH                    16
#define SW_LEFT_SIDE_RO_BUTTON                17
#define SW_ORBS_RIGHT_LANE_TARGET             18
#define SW_INLANE_BACK_TARGET                 19
#define SW_LEFT_&_RIGHT_FLIPPER_BUTTONS       20
#define SW_RESET_1_THROUGH_4_TARGETS_TARGET   21
#define SW_TOP_STOP_1_THROUGH_4_TARGET        23
#define SW_RIGHT_4_DROP_TARGET_4              24
#define SW_RIGHT_4_DROP_TARGET_3              25
#define SW_RIGHT_4_DROP_TARGET_2              26
#define SW_RIGHT_4_DROP_TARGET_1              27
#define SW_S_DROP_TARGET                      28
#define SW_B_DROP_TARGET                      29
#define SW_R_DROP_TARGET                      30
#define SW_O_DROP_TARGET                      31
#define SW_END_OF_TROUGH                      32
#define SW_10_POINT_REBOUND                   33
#define SW_RIGHT_SLINGSHOT                    36
#define SW_LEFT_SLINGSHOT                     37
#define SW_RIGHT_THUMPER_BUMPER               38
#define SW_LEFT_THUMPER_BUMPER                39
#define SW_1ST_INLINE_DROP_TARGET             40
#define SW_2ND_INLINE_DROP_TARGET             41
#define SW_3RD_INLINE_DROP_TARGET             42
#define SW_4TH_INLINE_DROP_TARGET             43
#define SW_RIGHT_OUTLANE                      44
#define SW_RIGHT_RETURN_LANE                  45
#define SW_LEFT_RETURN_LANE                   46
#define SW_LEFT_OUTLANE                       47

#define SOL_OUTHOLE_KICKER            0
#define SOL_KNOCKER                   1
#define SOL_INLINE_DROP_TARGET_RESET  2
#define SOL_4_RIGHT_DROP_TARGET_RESET 3
#define SOL_LEFT_THUMPER_BUMPER       4
#define SOL_RIGHT_THUMPER_BUMPER      5
#define SOL_LEFT_SLINGSHOT            6
#define SOL_RIGHT_SLINGSHOT           7
#define SOL_ORBS_TARGET_RESET         8
#define SOL_RIGHT_4_DROP_TARGETS_1    9
#define SOL_RIGHT_4_DROP_TARGETS_2    10
#define SOL_RIGHT_4_DROP_TARGETS_3    11
#define SOL_RIGHT_4_DROP_TARGETS_4    12
#define SOL_BALL_RELEASE              13
#define SOL_BALL_KICK_TO_PLAYFIELD    14
#define SOL_COIN_LOCKOUT_DOOR         15
#define SOL_K1_RELAY                  16
#define SOL_MAGNET                    17

/* #define SOLCONT_FLIPPERS      0x80 */
/* #define SOLCONT_COIN_LOCKOUT  0x01 */

#define NUM_SWITCHES_WITH_TRIGGERS          4
#define NUM_PRIORITY_SWITCHES_WITH_TRIGGERS 4

struct PlayfieldAndCabinetSwitch SolenoidAssociatedSwitches[] = {
  { SW_RIGHT_SLINGSHOT, SOL_RIGHT_SLINGSHOT, 4},
  { SW_LEFT_SLINGSHOT, SOL_LEFT_SLINGSHOT, 4},
  { SW_LEFT_THUMPER_BUMPER, SOL_LEFT_THUMPER_BUMPER, 3},
  { SW_RIGHT_THUMPER_BUMPER, SOL_RIGHT_THUMPER_BUMPER, 3}
};


#define LAMP_ANIMATION_STEPS  16
byte LampAnimations[3][LAMP_ANIMATION_STEPS][8] = {
  // Radial
  {{0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00},
{0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00},
{0x00, 0x00, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00},
{0x00, 0x55, 0x01, 0x10, 0x00, 0x81, 0x00, 0x00},
{0x00, 0xAA, 0x02, 0x10, 0x00, 0x41, 0x00, 0x00},
{0x00, 0x80, 0x02, 0x10, 0x00, 0x00, 0x00, 0x00},
{0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00},
{0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00},
{0x01, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00},
{0x00, 0x00, 0x00, 0x10, 0x00, 0x08, 0x00, 0x00},
{0x00, 0x00, 0x00, 0x10, 0x10, 0x38, 0x00, 0x00},
{0x08, 0x00, 0x00, 0x10, 0xA0, 0x04, 0x00, 0x00},
{0x12, 0x00, 0x00, 0x10, 0xC0, 0x04, 0x00, 0x00},
{0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00},
{0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00},
{0x04, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00}},
// Center out
  {{0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00},
{0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00},
{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
{0x00, 0x00, 0x03, 0x00, 0x80, 0x00, 0x00, 0x00},
{0x00, 0x80, 0x03, 0x00, 0x90, 0x00, 0x00, 0x00},
{0x05, 0xC0, 0x01, 0x00, 0x30, 0x00, 0x00, 0x00},
{0x05, 0x70, 0x00, 0x00, 0x60, 0x08, 0x00, 0x00},
{0x00, 0x3C, 0x00, 0x00, 0x40, 0x08, 0x00, 0x00},
{0x00, 0x07, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00},
{0x00, 0x01, 0x00, 0x00, 0x00, 0xE0, 0x00, 0x00},
{0x10, 0x00, 0x00, 0x00, 0x00, 0xE1, 0x00, 0x00},
{0x10, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00},
{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
{0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
// Bottom to top
  {{0x00, 0x00, 0x00, 0x00, 0x00, 0xC1, 0x00, 0x00},
{0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
{0x00, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
{0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
{0x00, 0x80, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00},
{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
{0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00},
{0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
{0x00, 0x00, 0x00, 0x00, 0x90, 0x00, 0x00, 0x00},
{0x00, 0x00, 0x00, 0x00, 0x60, 0x08, 0x00, 0x00},
{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
{0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00},
{0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00},
{0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
{0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}
};
