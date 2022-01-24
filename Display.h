#ifndef DISPLAY_H

byte MagnitudeOfScore(unsigned long score);
byte GetDisplayMask(byte numDigits);
void ShowPlayerScores(byte displayToUpdate, boolean flashCurrent, boolean dashCurrent, unsigned long allScoresShowValue = 0);

#define DISPLAY_H
#endif
