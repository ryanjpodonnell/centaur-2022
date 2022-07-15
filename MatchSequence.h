#ifndef MATCH_SEQUENCE_MODE_H
#define MATCH_SEQUENCE_MODE_H

class MatchSequence {
  private:
    byte          matchDigit_;
    byte          numMatchSpins_;
    byte          scoreMatches_;
    unsigned long matchDelay_;
    unsigned long matchSequenceStartTime_;

    void manageNewMode();

  public:
    MatchSequence();

    byte run(boolean currentStateChanged);
};

#endif
