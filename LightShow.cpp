#include "LightShow.h"
#include "PinballMachineBase.h"

int* allBonusLights() {
  static int test[] = {
    LAMP_1K_BONUS,
    LAMP_2K_BONUS,
    LAMP_3K_BONUS,
    LAMP_4K_BONUS,
    LAMP_5K_BONUS,
    LAMP_6K_BONUS,
    LAMP_7K_BONUS,
    LAMP_8K_BONUS,
    LAMP_9K_BONUS,
    LAMP_10K_BONUS,
    LAMP_20K_BONUS,
    LAMP_40K_BONUS,
    LAMP_60K_BONUS,
    LAMP_2X_BONUS,
    LAMP_3X_BONUS,
    LAMP_4X_BONUS,
    LAMP_5X_BONUS,
    LAMP_TERMINATOR
  };

  return test;
}
