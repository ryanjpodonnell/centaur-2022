#ifndef DISPLAY_H

byte GetDisplayMask(byte numDigits);
byte MagnitudeOfScore(unsigned long score);
void ShowPlayerScores(byte displayToUpdate, boolean flashCurrent, boolean dashCurrent, unsigned long allScoresShowValue = 0);
void StartScoreAnimation(unsigned long scoreToAnimate);

#define DISPLAY_H
#endif
