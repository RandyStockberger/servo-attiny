//
// ============================================================================
//
// servo.c -- Handle servo subsystem for program servoturnout.c
//
// ============================================================================
//
#include <stdint.h>
//#include <avr/eeprom.h>
#include <avr/pgmspace.h>
//
#include "servoturnout.h"
//
#include "servo.h"
#include "led.h"
#include "rom.h"
// ============================================================================
servoData_t servo[SERVO_COUNT] = {
	{ SERVO_DEFAULT_MIN, SERVO_DEFAULT_MIN, SERVO_DEFAULT_MAX, SERVO_DEFAULT_MIN, SERVO1_PIN },
	{ SERVO_DEFAULT_MIN, SERVO_DEFAULT_MIN, SERVO_DEFAULT_MAX, SERVO_DEFAULT_MIN, SERVO2_PIN },
};		// Data for each servo

uint8_t	lastServo = 0xFF;			// Last servo that had a button press
//
// ============================================================================
// servoPWMSet -- Set the new PWM value for the  servo
//
void servoPWMSet( enum eServo idx )
{
	if ( servo1 == idx ) {
		OCR1A = servo[idx].currentPos;
	}
	else if ( servo2 == idx ) {
		OCR1B = servo[idx].currentPos;
	}
}
//
// ============================================================================
// servoUpdateMinPos -- Update minPos in the servo data structure and in ROM
//
static void servoUpdateMinPos( enum eServo idx, uint16_t newPos )
{
	uint16_t * addr;

	if ( servo1 == idx )
		addr = (uint16_t *)ROM_ADDR_SERVO1_MINPOS;
	else
		addr = (uint16_t *)ROM_ADDR_SERVO2_MINPOS;
	romWrite( addr, newPos );

	servo[idx].minPos = newPos;
}
//
// ============================================================================
// servoUpdateMaxPos -- Update maxPos in the servo data structure and in ROM
//
static void servoUpdateMaxPos( enum eServo idx, uint16_t newPos )
{
	uint16_t * addr;

	if ( servo1 == idx )
		addr = (uint16_t *)ROM_ADDR_SERVO1_MAXPOS;
	else
		addr = (uint16_t *)ROM_ADDR_SERVO2_MAXPOS;
	romWrite( addr, newPos );

	servo[idx].maxPos = newPos;
}
//
// ============================================================================
// servoUpdateTargetPos -- Update targetPos in the servo data structure and in ROM
//
static void servoUpdateTargetPos( enum eServo idx, uint16_t newPos )
{
	uint16_t * addr;

	if ( servo1 == idx )
		addr = (uint16_t *)ROM_ADDR_SERVO1_TARGETPOS;
	else
		addr = (uint16_t *)ROM_ADDR_SERVO2_TARGETPOS;
	romWrite( addr, newPos );

	servo[idx].targetPos = newPos;
}
//
// ============================================================================
// servoLEDSet -- Set LEDs associated with this servo in accordance with the current state
// Servo moving: LDxA off, LDxB off
// Servo at minPos	LDxA on, LDxB off
// Servo at maxPos	LDxA off, LDxB on
void servoLEDSet( enum eServo idx )
{
	switch ( idx ) {
		case servo1:
			ledOff( LD1A );
			ledOff( LD1B );
			if ( servo[idx].currentPos == servo[idx].minPos ) {
				ledOn ( LD1A );
			}
			else if ( servo[idx].currentPos == servo[idx].maxPos ) {
				ledOn ( LD1B );
			}
			break;

		case servo2:
			ledOff( LD2A );
			ledOff( LD2B );
			if ( servo[idx].currentPos == servo[idx].minPos ) {
				ledOn ( LD2A );
			}
			else if ( servo[idx].currentPos == servo[idx].maxPos ) {
				ledOn ( LD2B );
			}
			break;

		default:
			break;
	}

}
//
// ============================================================================
// servoMove -- increment/decrement the currentPos of each servo that is in motion
void servoMove( void )
{
	enum eServo idx;

// TODO: Add a hook here to disable servo when idle and enable when active...

	for (idx=0; idx<SERVO_COUNT; ++idx ) {
		// Apply delta to each turnout that is in motion
		if ( servo[idx].currentPos < servo[idx].targetPos ) {
			servo[idx].currentPos += SERVO_DELTA;
			// Check for overshoot
			if ( servo[idx].currentPos > servo[idx].targetPos )
				servo[idx].currentPos = servo[idx].targetPos;
		}
		else if ( servo[idx].currentPos > servo[idx].targetPos ) {
			servo[idx].currentPos -= SERVO_DELTA;
			// Check for undershoot
			if ( servo[idx].currentPos < servo[idx].targetPos )
				servo[idx].currentPos = servo[idx].targetPos;
		}
		servoPWMSet( idx );		// Write currentPos to the control register
		servoLEDSet( idx );
		// Set/Reset the LED
	}
}
// ============================================================================
// servoToggle -- Reverse servo position
//
void servoToggle( enum eServo idx )
{
	uint16_t newPos;


	if ( servo[idx].targetPos == servo[idx].maxPos ) {
		newPos = servo[idx].minPos;
	}
	else {
		newPos = servo[idx].maxPos;
	}
	servoUpdateTargetPos( idx, newPos );

	lastServo = idx;		// Store index of most recent servo
}
//
// ============================================================================
// servoWiden -- Increase limit of current position of most recently toggled servo
//
// Do not go beyond the current minimum/maximum
//
extern void servoWiden( void )
{
	uint16_t newPos;
	uint16_t * addr;

	if ( lastServo < SERVO_COUNT ) {
		if ( servo[lastServo].targetPos == servo[lastServo].minPos ) {
			newPos = servo[lastServo].minPos - SERVO_LIMIT_DELTA;
			// Clip newpos to the absolute minimum limit
			if ( newPos < SERVO_ABSOLUTE_MIN ) {
				newPos = SERVO_ABSOLUTE_MIN;
			}

			// !Never! cross the streams
			if ( newPos < servo[lastServo].maxPos ) {
				servoUpdateTargetPos( lastServo, newPos );
				servoUpdateMinPos( lastServo, newPos );
			}
		}
		else if ( servo[lastServo].targetPos == servo[lastServo].maxPos ) {
			newPos = servo[lastServo].maxPos + SERVO_LIMIT_DELTA;
			// Clip newpos to the absolute maximum limit
			if ( newPos > SERVO_ABSOLUTE_MAX ) {
				newPos = SERVO_ABSOLUTE_MAX;
			}
			if ( newPos > servo[lastServo].minPos ) {
				servoUpdateTargetPos( lastServo, newPos );
				servoUpdateMaxPos( lastServo, newPos );
			}
		}
	}
}
//
// ============================================================================
// servoNarrow -- Decrease limit of current position of most recently toggled servo
//
// Do not go beyond the current minimum/maximum
//
extern void servoNarrow( void )
{
	uint16_t newPos;

	if ( lastServo < SERVO_COUNT ) {
		if ( servo[lastServo].targetPos == servo[lastServo].minPos ) {
			newPos = servo[lastServo].minPos + SERVO_LIMIT_DELTA;
			// Clip newpos to the absolute maximum limit
			if ( newPos > SERVO_ABSOLUTE_MAX ) {
				newPos = SERVO_ABSOLUTE_MAX;
			}

			// !Never! cross the streams
			if ( newPos < servo[lastServo].maxPos ) {
				servoUpdateMinPos( lastServo, newPos );
				servoUpdateTargetPos( lastServo, newPos );
			}
		}
		else if ( servo[lastServo].targetPos == servo[lastServo].maxPos ) {
			newPos = servo[lastServo].maxPos - SERVO_LIMIT_DELTA;
			// Clip newpos to the absolute minimum limit
			if ( newPos < SERVO_ABSOLUTE_MIN ) {
				newPos = SERVO_ABSOLUTE_MIN;
			}

			if ( newPos > servo[lastServo].minPos ) {
				servoUpdateMaxPos( lastServo, newPos );
				servoUpdateTargetPos( lastServo, newPos );
			}
		}
	}
}
//
// ============================================================================
//
