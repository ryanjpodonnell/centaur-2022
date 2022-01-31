#ifndef SKILL_SHOT_MODE_H
#define SKILL_SHOT_MODE_H

class SkillShot {
  private:
    byte activeRollover_;

    void handleNewMode();

  public:
    SkillShot();

    byte run(boolean currentStateChanged, byte switchHit);
};

#endif
