#ifndef HERTZ_LOGGING_H
#define HERTZ_LOGGING_H

class HertzLogging {
  private:
    unsigned long numLoops_ = 0;
    unsigned long lastLoopReportTime_ = 0;

  public:
    HertzLogging();

    void run();
};

#endif
