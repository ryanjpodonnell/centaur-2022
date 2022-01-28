#ifndef DEBUG_H
#define DEBUG_H

#define PINBALL_MACHINE_BASE_MAJOR_VERSION  2022
#define PINBALL_MACHINE_BASE_MINOR_VERSION  1

class Debug {
  private:
    unsigned long stateStatedTime_;

  public:
    Debug();

    byte run(byte currentState, boolean currentStateChanged);
};

#endif
