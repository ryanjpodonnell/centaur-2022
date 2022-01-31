#ifndef SKILL_SHOT_MODE_H
#define SKILL_SHOT_MODE_H

class SkillShot {
  private:
    byte litRollover_;

    void handleNewMode();

  public:
    SkillShot();

    byte run(byte gameModeId, boolean currentStateChanged);
};

#endif
