#ifndef BASE_MODE_H
#define BASE_MODE_H

class Base {
  private:
    byte inlineMultiplier();
    void manageDefaultScoringLogic(unsigned long value, byte switchHit);
    void manageInlineTargetLogic(byte defaultSound, byte switchHit);

  public:
    Base();

    int run(byte switchHit);
};

#endif
