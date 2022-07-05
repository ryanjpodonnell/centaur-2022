#ifndef RESTART_GAME_H
#define RESTART_GAME_H

class RestartGame {
  private:
    void manageNewState();

  public:
    RestartGame();

    int run(boolean currentStateChanged);
};

#endif
