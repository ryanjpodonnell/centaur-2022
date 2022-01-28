#ifndef LAMPS_H
#define LAMPS_H

#define LAMP_TERMINATOR                 255
#define LAMP_PLAYFIELD_GI               0
#define LAMP_RIGHT_LANE_RELEASE_ORBS    2
#define LAMP_COLLECT_BONUS_ARROW        3
#define LAMP_1K_BONUS                   4
#define LAMP_2K_BONUS                   5
#define LAMP_3K_BONUS                   6
#define LAMP_4K_BONUS                   7
#define LAMP_5K_BONUS                   8
#define LAMP_6K_BONUS                   9
#define LAMP_7K_BONUS                   10
#define LAMP_8K_BONUS                   11
#define LAMP_9K_BONUS                   12
#define LAMP_10K_BONUS                  13
#define LAMP_CHAMBERS_RELEASE           14
#define LAMP_CHAMBERS_SPECIAL           15
#define LAMP_2X_BONUS                   16
#define LAMP_3X_BONUS                   17
#define LAMP_4X_BONUS                   18
#define LAMP_5X_BONUS                   19
#define LAMP_RIGHT_OUT_ROLLOVER         20
#define LAMP_RIGHT_RETURN_ROLLOVER      21
#define LAMP_LEFT_RETURN_ROLLOVER       22
#define LAMP_LEFT_OUT_ROLLOVER          23
#define LAMP_RIGHT_DROP_TARGET_80_ARROW 24
#define LAMP_RIGHT_DROP_TARGET_40_ARROW 25
#define LAMP_RIGHT_DROP_TARGET_20_ARROW 26
#define LAMP_RIGHT_DROP_TARGET_10_ARROW 27
#define LAMP_S_DROP_TARGET_ARROW        28
#define LAMP_B_DROP_TARGET_ARROW        28
#define LAMP_R_DROP_TARGET_ARROW        28
#define LAMP_O_DROP_TARGET_ARROW        28
#define LAMP_10_CHAMBER                 32
#define LAMP_20_CHAMBER                 33
#define LAMP_30_CHAMBER                 34
#define LAMP_40_CHAMBER                 35
#define LAMP_50_CHAMBER                 36
#define LAMP_20K_BONUS                  37
#define LAMP_40K_BONUS                  38
#define LAMP_60K_BONUS                  39
#define LAMP_1_CAPTIVE_ORBS             44
#define LAMP_2_CAPTIVE_ORBS             45
#define LAMP_3_CAPTIVE_ORBS             46
#define LAMP_4_CAPTIVE_ORBS             47
#define LAMP_ORBS_SPECIAL               52
#define LAMP_ORBS_50K                   53
#define LAMP_RESET_1_THROUGH_4_ARROW    54
#define LAMP_SPOT_1_THROUGH_4           55
#define LAMP_RIGHT_LANE_2X              56
#define LAMP_RIGHT_LANE_3X              57
#define LAMP_RIGHT_LANE_4X              58
#define LAMP_RIGHT_LANE_5X              59
#define LAMP_TOP_RIGHT_ROLLOVER         60
#define LAMP_TOP_MIDDLE_ROLLOVER        61
#define LAMP_TOP_LEFT_ROLLOVER          62
#define LAMP_RIGHT_SLINGSHOT_GI         64
#define LAMP_LEFT_SLINGSHOT_GI          65
#define LAMP_RIGHT_THUMPER_BUMPER       66
#define LAMP_LEFT_THUMPER_BUMPER        67
#define LAMP_QUEENS_CHAMBER_GI_1        68
#define LAMP_QUEENS_CHAMBER_GI_2        69
#define LAMP_QUEENS_CHAMBER_GI_3        70
#define LAMP_QUEENS_CHAMBER_GI_4        71

#define LAMP_COLLECTION_BONUS_ALL         0
#define LAMP_COLLECTION_BONUS_MIDDLE_RING 1
#define LAMP_COLLECTION_BONUS_OUTER_RING  2
#define LAMP_COLLECTION_RING_4            3
#define LAMP_COLLECTION_RING_5            4
#define LAMP_COLLECTION_RING_6            5

static byte bonusAllLamps_[] = {
  LAMP_1K_BONUS,
  LAMP_2K_BONUS,
  LAMP_3K_BONUS,
  LAMP_4K_BONUS,
  LAMP_5K_BONUS,
  LAMP_6K_BONUS,
  LAMP_7K_BONUS,
  LAMP_8K_BONUS,
  LAMP_9K_BONUS,
  LAMP_10K_BONUS,
  LAMP_20K_BONUS,
  LAMP_40K_BONUS,
  LAMP_60K_BONUS,
  LAMP_2X_BONUS,
  LAMP_3X_BONUS,
  LAMP_4X_BONUS,
  LAMP_5X_BONUS,
  LAMP_TERMINATOR
};

static byte bonusMiddleRingLamps_[] = {
  LAMP_20K_BONUS,
  LAMP_60K_BONUS,
  LAMP_2X_BONUS,
  LAMP_3X_BONUS,
  LAMP_4X_BONUS,
  LAMP_5X_BONUS,
  LAMP_TERMINATOR
};

static byte bonusOuterRingLamps_[] = {
  LAMP_1K_BONUS,
  LAMP_2K_BONUS,
  LAMP_3K_BONUS,
  LAMP_4K_BONUS,
  LAMP_5K_BONUS,
  LAMP_6K_BONUS,
  LAMP_7K_BONUS,
  LAMP_8K_BONUS,
  LAMP_9K_BONUS,
  LAMP_10K_BONUS,
  LAMP_TERMINATOR
};

static byte ring4Lamps_[] = {
  LAMP_2_CAPTIVE_ORBS,
  LAMP_3_CAPTIVE_ORBS,
  LAMP_10_CHAMBER,
  LAMP_TERMINATOR
};

static byte ring5Lamps_[] = {
  LAMP_1_CAPTIVE_ORBS,
  LAMP_4_CAPTIVE_ORBS,
  LAMP_LEFT_RETURN_ROLLOVER,
  LAMP_RIGHT_DROP_TARGET_80_ARROW,
  LAMP_RIGHT_RETURN_ROLLOVER,
  LAMP_TERMINATOR
};

static byte ring6Lamps_[] = {
  LAMP_1_CAPTIVE_ORBS,
  LAMP_4_CAPTIVE_ORBS,
  LAMP_LEFT_RETURN_ROLLOVER,
  LAMP_RIGHT_DROP_TARGET_80_ARROW,
  LAMP_RIGHT_RETURN_ROLLOVER,
  LAMP_TERMINATOR
};

class LampsHelper {
  private:

  public:
    LampsHelper();

    void showLamps(byte lampCollection, bool clearAllLamps = false);
    void showLamp(byte lamp, bool clearAllLamps = false);
    void showAllLamps();
};

#endif
