#ifndef DEBUG_H
#define DEBUG_H

#define PINBALL_MACHINE_BASE_MAJOR_VERSION  2022
#define PINBALL_MACHINE_BASE_MINOR_VERSION  10

class Debug {
  private:
    unsigned long stateStartedTime_;

    void manageNewState();

  public:
    Debug();

    int run(boolean currentStateChanged);
};

#endif
