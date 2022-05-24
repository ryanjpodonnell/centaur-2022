#ifndef SKILL_SHOT_MODE_H
#define SKILL_SHOT_MODE_H

class SkillShot {
  private:
    byte          activeRollover_;
    byte          bonusValue_;
    unsigned long scoreValue_;

    byte checkRollover(byte switchHit);
    void handleNewMode();

  public:
    SkillShot();

    byte run(boolean currentStateChanged, byte switchHit);
};

#endif
