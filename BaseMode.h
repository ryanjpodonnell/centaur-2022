#ifndef BASE_MODE_H
#define BASE_MODE_H

class Base {
  private:
    void handleNewMode();

  public:
    Base();

    int run(byte switchHit);
};

#endif
