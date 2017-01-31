// ============================================================================
//
// rom.c -- Handle backing store (eeprom or flash)
//
// ============================================================================
#include <stdint.h>
#include <avr/eeprom.h>
//
#include "servo.h"
#include "rom.h"
//
//
// ============================================================================
// Structure for the data on the ROM
//
/*
typedef struct {
	uint16_t	signature;			// A signature to insure we got valid data from the ROM
	uint16_t	eepromversion;		// The version of eeprom data format
	servoData_t	servoData[SERVO_COUNT];		// Data for individual servos
} servoEeprom_t;
*/
//
// ============================================================================
// romWrite -- Write one word to the appropriate backing store location
//
void romWrite( uint16_t * addr, uint16_t value )
{
	eeprom_update_word( addr, value );
}
//
// ============================================================================
// romCheckRange -- Verify that a value is within the allowed range
//
uint16_t romCheckRange( uint16_t curValue, uint16_t maxValue, uint16_t minValue )
{
	if ( curValue > maxValue ) {
		curValue = maxValue;
	}
	if ( curValue < minValue ) {
		curValue = minValue;
	}

	return curValue;
}
// ============================================================================
// romServoDataInitialize -- Initialize servo data from persistent storage
//
// Check ROM signature and version.
// If correct and current read ROM data into the servo structure
// If out of date or incorrect, write default data to ROM and servo structure
// ROM values are range checked and corrected if they are beyond absolute limits
//
void romServoDataInitialize( void )
{
	uint16_t sig;
	uint16_t ver;
	uint8_t idx;
	
	sig = eeprom_read_word( (uint16_t *)ROM_ADDR_SIGNATURE );
	ver = eeprom_read_word( (uint16_t *)ROM_ADDR_EEVERSION );

	// If signature and version are correct fetch from ROM
	if ( (sig == SERVO_SIGNATURE) && (ver == SERVO_EEVERSION) ) {
		// TODO: Add error checking to targetPos and min/maxPos
		servo[0].targetPos = eeprom_read_word( (uint16_t *)ROM_ADDR_SERVO1_TARGETPOS );
		servo[0].targetPos = romCheckRange( servo[0].targetPos,
			   				SERVO_ABSOLUTE_MAX, SERVO_ABSOLUTE_MIN );

		servo[0].minPos = eeprom_read_word( (uint16_t *)ROM_ADDR_SERVO1_MINPOS );
		servo[0].minPos = romCheckRange( servo[0].minPos,
			   				SERVO_ABSOLUTE_MAX, SERVO_ABSOLUTE_MIN );

		servo[0].maxPos = eeprom_read_word( (uint16_t *)ROM_ADDR_SERVO1_MAXPOS );
		servo[0].maxPos = romCheckRange( servo[0].maxPos,
			   				SERVO_ABSOLUTE_MAX, SERVO_ABSOLUTE_MIN );

		servo[0].currentPos = servo[0].targetPos;
		servo[0].Pin = SERVO1_PIN;
			servo[0].targetPos = SERVO_DEFAULT_MIN;
		//
		servo[1].targetPos = eeprom_read_word( (uint16_t *)ROM_ADDR_SERVO2_TARGETPOS );
		servo[1].targetPos = romCheckRange( servo[1].targetPos,
			   				SERVO_ABSOLUTE_MAX, SERVO_ABSOLUTE_MIN );

		servo[1].minPos = eeprom_read_word( (uint16_t *)ROM_ADDR_SERVO2_MINPOS );
		servo[1].minPos = romCheckRange( servo[1].minPos,
			   				SERVO_ABSOLUTE_MAX, SERVO_ABSOLUTE_MIN );

		servo[1].maxPos = eeprom_read_word( (uint16_t *)ROM_ADDR_SERVO2_MAXPOS );
		servo[1].maxPos = romCheckRange( servo[1].maxPos,
			   				SERVO_ABSOLUTE_MAX, SERVO_ABSOLUTE_MIN );

		servo[1].currentPos = servo[1].targetPos;
		servo[1].Pin = SERVO2_PIN;
		//
	}
	else {
		
		// Bad signature or version
		// The servo[] array was initialized at startup
		// Initialize ROM with default, minPos, maxPos and targetPos
		//
/*
		// Initialize servo data structure with the defaults
		servo[0].targetPos = SERVO_DEFAULT_MIN;
		servo[0].minPos = SERVO_DEFAULT_MIN;
		servo[0].maxPos = SERVO_DEFAULT_MAX;
		servo[0].currentPos = servo[0].targetPos;
		servo[0].Pin = SERVO1_PIN;
		//
		servo[1].targetPos = SERVO_DEFAULT_MIN;
		servo[1].minPos = SERVO_DEFAULT_MIN;
		servo[1].maxPos = SERVO_DEFAULT_MAX;
		servo[1].currentPos = servo[1].targetPos;
		servo[1].Pin = SERVO2_PIN;
*/
		//
		// Write signature and version to ROM
		eeprom_update_word( (uint16_t *)ROM_ADDR_SIGNATURE, ROM_SIGNATURE );
		eeprom_update_word( (uint16_t *)ROM_ADDR_EEVERSION, ROM_EEVERSION );
		// Write default servo data to ROM
		eeprom_update_word( (uint16_t *)ROM_ADDR_SERVO1_MINPOS, SERVO_DEFAULT_MIN );
		eeprom_update_word( (uint16_t *)ROM_ADDR_SERVO1_MAXPOS, SERVO_DEFAULT_MAX );
		eeprom_update_word( (uint16_t *)ROM_ADDR_SERVO1_TARGETPOS, SERVO_DEFAULT_MIN );

		eeprom_update_word( (uint16_t *)ROM_ADDR_SERVO2_MINPOS, SERVO_DEFAULT_MIN );
		eeprom_update_word( (uint16_t *)ROM_ADDR_SERVO2_MAXPOS, SERVO_DEFAULT_MAX );
		eeprom_update_word( (uint16_t *)ROM_ADDR_SERVO2_TARGETPOS, SERVO_DEFAULT_MIN );
	}
}
//
// ============================================================================
//
