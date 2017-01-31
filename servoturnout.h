
#ifndef _SERVOTURNOUT_H_
#define _SERVOTURNOUT_H_
//
#define FCPU				8000000UL	// Default internal oscillator is 8MHz
#define SERVO_HZ			50L
#define TIMER1_DIVISOR		8L
#define PWMTOP				(FCPU/((TIMER1_DIVISOR)*(SERVO_HZ)))

typedef enum { ePORTUnknown, ePORTB, ePORTC, ePORTD } ePorts_t;
//
extern const char mydata[] PROGMEM;
extern uint8_t eepromUpdateFlag;
//
#endif	// _SERVOTURNOUT_H_
