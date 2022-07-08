#ifndef DESTROY_CENTAUR_H
#define DESTROY_CENTAUR_H

class DestroyCentaur {
  private:
    boolean       overrideBaseSound_;
    unsigned long jackpotValue_;
    unsigned long lastFlash_;

    byte endMode();
    void manageModeLamps();
    void manageNewMode();

  public:
    DestroyCentaur();

    boolean       overrideBaseSound();
    byte          run(boolean currentStateChanged, byte switchHit);
    unsigned long jackpotValue();
    void          endModeViaBallEnded();
};

#endif
