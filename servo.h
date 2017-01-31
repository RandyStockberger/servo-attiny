//
#ifndef _SERVO_H_
#define _SERVO_H_
//
// ============================================================================
//
// servo.h -- Definitions and declarations for the servo subsystem
//
// ============================================================================
//
#define EEVERSION_MAJOR			0
#define EEVERSION_MINOR			1
//
#define SERVO_SIGNATURE			0x4545	// EEPROM Data signature 'EE'
#define SERVO_EEVERSION			((EEVERSION_MAJOR<<8)|(EEVERSION_MINOR))
//
// Drive servo(s) via 16 bit PWM
// On the ATTiny2313/ATTiny4313 these are timer1 and are on pins OC1A on PB3 and OC1B on PB4
//
#define SERVO_PORT			PORTB
#define SERVO_DDR			DDRB

#define SERVO_COUNT			2
#define SERVO1_PIN			3
#define SERVO2_PIN			4
//
// To prevent slamming the servo from it's old position to the new position the current
// position is incremented on each heartbeat interrupt by SERVO_DELTA until it reaches
// the new position.
#define SERVO_DELTA			8
//
// The servo open and closed positions are adjustable by pressing BTNPLUS and BTNMINUS.
// SERVO_LIMIT_DELTA defines how much minPos/maxPos is changed each time the button is
// pressed.
#define SERVO_LIMIT_DELTA		10
//
// The 'new out of the box' values for the opened and closed positions
#define SERVO_DEFAULT_MIN		1200
#define SERVO_DEFAULT_CENTER	1500
#define SERVO_DEFAULT_MAX		1800
//
// The absolute limits that the servo can't be allowed to exceed
#define SERVO_ABSOLUTE_MIN		1000
#define SERVO_ABSOLUTE_MAX		2000
//
enum eServo { servo1, servo2 };
//
// ============================================================================
// EEPROM data structure to preserve servo states between sessions
typedef struct {
	uint16_t	targetPos;			// Final position of the servo, minPos or maxPos
	uint16_t	minPos;			// Servo's value for 'min' position
	uint16_t	maxPos;			// Servo's value for 'max' position
	uint16_t	currentPos;			// Current position for this servo
	uint16_t	Pin;				// This servo's Output pin
} servoData_t;
//
extern servoData_t servo[SERVO_COUNT];
//
//extern const uint8_t jervoPins[];
//
//void configServoTimer(void);
void servoMove( void );
void servoToggle( enum eServo idx );
void servoWiden( void );
void servoNarrow( void );
//
// ============================================================================
//
#endif	// _SERVO_H_
