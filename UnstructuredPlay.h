#ifndef UNSTRUCTURED_PLAY_H
#define UNSTRUCTURED_PLAY_H

class UnstructuredPlay {
  private:
    void    handleNewMode();

  public:
    UnstructuredPlay();

    byte run(boolean currentStateChanged, byte switchHit);
};

#endif
