#include "atmegatron.h"


// The Arduino map function.
long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


int round(double number)
{
    return (number >= 0) ? (int)(number + 0.5) : (int)(number - 0.5);
}


void randomSeed(unsigned int seed)
{
  if (seed != 0)
  {
    srand(seed);
  }
}


// Adapted from Arduino.
static long random_(long howbig)
{
  if (howbig == 0)
  {
    return 0;
  }
  return rand() % howbig;
}


// Adapted from Arduino.
long random(long howsmall, long howbig)
{
  if (howsmall >= howbig)
  {
    return howsmall;
  }
  long diff = howbig - howsmall;
  return random_(diff) + howsmall;
}


void cli(void)
{
}


void sei(void)
{
}
