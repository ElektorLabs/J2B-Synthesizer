#ifndef __UTIL_H__
#define __UTIL_H__


// From Arduino.h
#define bitRead(value,bit)  (((value) >> (bit)) & 0x01)
#define bitSet(value,bit)  ((value) |= (1UL << (bit)))
#define bitClear(value,bit)  ((value) &= ~(1UL << (bit)))
#define bitWrite(value,bit, bitvalue)  (bitvalue ? bitSet(value,bit) : bitClear(value,bit))


long map(long x, long in_min, long in_max, long out_min, long out_max);
int round(double number);
void randomSeed(unsigned int seed);
long random(long howsmall, long howbig);
void cli(void);
void sei(void);

// Other useful math stuff.
#define abs(a)  ((a)>=0? (a) : -(a))
#define SQRT2  (1.414213562373095f)    //squareroot 2 - useful!
#define PI  (3.141592653589793f)


#endif // __UTIL_H__
