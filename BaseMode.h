#ifndef BASE_MODE_H
#define BASE_MODE_H

class Base {
  private:
    void manageDefaultScoringLogic(unsigned long value);
    void manageSignificantSwitchHit();

  public:
    Base();

    int run(byte switchHit);
};

#endif
