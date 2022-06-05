#ifndef BONUS_LIGHT_SHOW_H
#define BONUS_LIGHT_SHOW_H

#define BONUS_LIGHT_SHOW_ORBS_AND_RIGHT_DROPS_ARROW  0
#define BONUS_LIGHT_SHOW_ORBS_DROPS_ARROW            1
#define BONUS_LIGHT_SHOW_RESET_1_THROUGH_4_ARROW     2
#define BONUS_LIGHT_SHOW_RIGHT_DROPS_ARROW           3
#define BONUS_LIGHT_SHOW_SPIN                        4

static int showDurations_[] = {
  6000, // 0
  3000, // 1
  3000, // 2
  3000, // 3
  1800  // 4
};

class BonusLightShow {
  private:
    boolean       ended_;
    boolean       running_;
    byte          lightShowId_;
    unsigned long endTime_;
    unsigned long lastFlash_;

    void manageOrbsAndRightDropsArrowShow();
    void manageOrbsDropsArrowShow();
    void manageResetArrowShow();
    void manageRightDropsArrowShow();
    void manageSpinShow();

  public:
    BonusLightShow();

    boolean ended();
    boolean running(); 
    void    end();
    void    reset();
    void    run();
    void    start(byte lightShowId);
};

#endif
