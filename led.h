
#ifndef _LED_H_
#define _LED_H_

// ============================================================================
//
// led.h -- Provide LED functions to the servoturnout program
//
// ============================================================================
//
// LED to PORT/PIN mapping - ATTinyx313:
//
// LD1A		PB0	-- Servo 1 state A LED
// LD1B		PB1	-- Servo 1 state B LED
// LED1		PB2 -- Debug LED
// LED2		PD4 -- Reserved pin for an additional debug LED
// LD2A		PD5	-- Servo 2 state A LED
// LD2B		PD6	-- Servo 2 state B LED
//
// ============================================================================
//
enum eLED { LD1A, LD1B, LD2A, LD2B, LED1, LED2 };
//
#define LED_COUNT	5
//
// PORTB LEDs
#define	LD1A_PIN		0
#define	LD1B_PIN		1
#define	LED1_PIN		2
//
// PORTD LEDs
#define	LD2A_PIN		5
#define	LD2B_PIN		6
#define	LED2_PIN		4
//
//extern led_t leds[LED_COUNT];
//
extern void ledConfig( void );
extern void ledOff( enum eLED idx );
extern void ledOn( enum eLED idx );
//
// ============================================================================
#endif	// _LED_H_
