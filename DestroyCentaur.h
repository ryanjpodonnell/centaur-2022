#ifndef DESTROY_CENTAUR_H
#define DESTROY_CENTAUR_H

class DestroyCentaur {
  private:
    unsigned long jackpotValue_;

    byte endMode();
    void manageModeLamps();
    void manageNewMode();

  public:
    DestroyCentaur();

    byte          run(boolean currentStateChanged, byte switchHit);
    unsigned long jackpotValue();
    void          endModeViaBallEnded();
};

#endif
