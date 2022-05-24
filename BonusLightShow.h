#ifndef BONUS_LIGHT_SHOW_H
#define BONUS_LIGHT_SHOW_H

class BonusLightShow {
  private:
    boolean       running_;
    unsigned long endTime_;
    unsigned long lastFlash_;

  public:
    BonusLightShow();

    boolean ended();
    boolean running(); 
    void    end();
    void    run();
    void    start();
};

#endif
