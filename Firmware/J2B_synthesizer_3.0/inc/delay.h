#ifndef __DELAY_H__
#define __DELAY_H__


#define DELAY_SIZE (1280)

extern volatile int delay_writepos;
extern volatile int delay_readpos;
extern volatile sample_t delay_buffer[DELAY_SIZE];
extern volatile byte delay_fbmsb;
extern volatile byte delay_fblsb;

void Delay_Let_Position(byte newposition);
byte Delay_Get_Position(void);
void Delay_Let_Feedback(byte newfb);
inline byte Delay_Get_Feedback(void);
void Delay_Let_WetMix(byte newwm);
byte Delay_Get_WetMix(void);
void Delay_Let_DryMix(byte newdm);
byte Delay_Get_DryMix(void);
void Delay_Let_LFOAmt(byte newamt);
byte Delay_Get_LFOAmt(void);
void Delay_Let_FenvAmt(byte newamt);
byte Delay_Get_FenvAmt(void);
byte Delay_Get_SF(void);
void Delay_Let_SF(byte newamt);
boolean Delay_Get_ClipMode(void);
void Delay_Let_ClipMode(boolean newcm);
void Delay_CalcSF(void);
byte Delay_Get_LFOGain(void);
byte Delay_Get_FenvGain(void);


#endif // __DELAY_H__
