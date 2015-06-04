#ifndef __HARDWARE_PORT_H__
#define __HARDWARE_PORT_H__


#define HIGH  (1)
#define LOW  (0)


void Init_Hardware(void);
void Hardware_Poll(void);
void Hardware_LED_SetState(byte LEDnum, byte LEDstate);
void Hardware_LED_StartFlash(byte LEDnum, byte FlashTimes);
void Hardware_LED_CancelFlash(byte LEDnum);
void Hardware_BicolorLED_SetState(uint8_t led, uint8_t green, uint8_t red);
void Hardware_Let_CtrlBank(byte value);
void Hardware_Refresh_ValBG(void);
boolean Hardware_Ctrl(byte shift, uint8_t ctrl, int ctrl_min, int ctrl_max, boolean accelerate);


#if PRODUCT==ATMEGADRUM

void Hardware_Refresh_valLED(void);
void Hard_Let_Shift(boolean newshift);
void Hardware_Let_Value(byte newval);
byte Hardware_Get_Value(void);
void Hardware_Let_Ctrl(byte shift, byte index, byte value);
byte Hardware_Get_Ctrl(byte shift, byte index);
byte Hardware_Get_CtrlBank(void);
void Hardware_Refresh_CtrlBankLED(void);
void Hardware_Let_Function(byte newfunc);
byte Hardware_Get_Function(void);
void Hardware_Refresh_FuncBG(void);
void Hardware_Let_Value(byte newval);
byte Hardware_Get_Value(void);
void Hard_Let_Shift(boolean newshift);
boolean Hard_Get_Shift(void);
void Hardware_Refresh_ShiftLED(void);
void Hardware_LED_RefreshFlash(void);

#else /* PRODUCT */

void Hardware_Let_Ctrl(byte shift, byte index, byte value);
byte Hardware_Get_Ctrl(byte shift, byte index);
byte Hardware_Get_Ctrl_Shift(void);
void Hardware_Let_Function(byte newfunc);
byte Hardware_Get_Function();
void Hardware_Let_Value(byte func, byte newval);
byte Hardware_Get_Value(byte func);
boolean Hard_Get_Shift(byte func);
void Hard_Let_Shift(byte func, boolean newshift);

#endif /* PRODUCT */

#if PRODUCT==ATCYCLOTRON
unsigned int Hardware_Get_envADR(byte index);
char Hardware_Get_envS(byte index);
#endif /* PRODUCT */


extern uint8_t encoder_to_ctrl[8];


#endif // __HARDWARE_PORT_H__
