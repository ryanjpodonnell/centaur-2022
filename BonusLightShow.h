#ifndef BONUS_LIGHT_SHOW_H
#define BONUS_LIGHT_SHOW_H

#define BONUS_LIGHT_SHOW_SPIN                    0
#define BONUS_LIGHT_SHOW_RESET_1_THROUGH_4_ARROW 1

static int showDurations_[] = {
  1800, // 0
  1000  // 1
};

class BonusLightShow {
  private:
    boolean       running_;
    byte          lightShowId_;
    unsigned long endTime_;
    unsigned long lastFlash_;

    void manageResetArrowShow(unsigned long seed);
    void manageSpinShow(unsigned long seed);

  public:
    BonusLightShow();

    boolean ended();
    boolean running(); 
    void    end();
    void    run();
    void    start(byte lightShowId);
};

#endif
