//
// servoturnout.c
//
// Model railroad turnout control based on RC Servo
// Features:
// - Generate the PWM signals to drive two RC servos to control the turnout position
// - Slow speed on servo motion to prevent slamming the point rails into the thru rails
// - Independently adjustable limits on each servo to prevent overdriving the servo
// - Two LEDs per servo to indicate turnout position
// - Maintain correct frog polarity by powering the frog from either Rail-A or Rail-B
//
// ============================================================================
// turn servos slowly to prevent wear and tear
// Design an interface to allow adjustable servo limits
//	 Idea 1: Design a command interface running on the UART
//
//	 Idea 2: Design an interface based on LEDs and buttons
//	 	Add two buttons, 'BTN+' and 'BTN-' to the PC board. When BTN+ is
//	 	pressed increase the range of the most recently toggled servo in the
//	 	current direction. When BTN- is pressed, decrease the range.
//	 	For example, if the turnout connected to SERVO3 is pressing with too
//	 	much pressure when set to the divirging route and you wish to reduce
//	 	the force on the thru rail you would:
//	 	- Set the turnout to the straight route (if necessary), then back to
//	 	  the divirging route.
//			 			- Press the BTN- button one or more times to reduce the throw of the
//			 			  turnout in the divirging direction.
//			 			- Cycle the turnout to straight and back to divirging to make sure
//			 			  it moves to the divirging rail but does not use too much force.
//			 			- Repeat as needed for proper operation.
//
// ============================================================================
//
//  ATTinyx313 - 20 pin microcontroller
//
// Pin Assignments:
//
//	 SOIC Pin			Function			Usage
//	 --------			--------			-----
//		  1				~RESET				RESET
//		  2				PD0					BTPLUS
//		  3				PD1					BTMINUS
//		  4				PA1, XTAL2			NA
//		  5				PA0, XTAL1			NA
//		  6				PD2					BTS1
//		  7				PD3					BTS2
//		  8				PD4					LED2
//		  9				PD5					LD2A
//		 10				GND					GND
//		 11				PD6					LD2B
//		 12				PB0					LD1A
//		 13				PB1					LD1B
//		 14				PB2					LED1
//		 15				OC1A, PB3			SV1
//		 16				OC1B, PB4			SV2
//		 17				PB5,MOSI			MOSI
//		 18				PB6,MISO			MISO
//		 19				PB7,SCK				SCK
//		 20				VCC					V5
//
// ============================================================================
//
// Setup():
// Configure LEDs on PD4, PD5, PD6, PB0, PB1 and PB2 as outputs
// Configure Buttons on PD0, PD1, PD2 and PD3 as inputs
// Configure Servo drive signals on PB3 and PB4 as Timer 1 PWM
// ============================================================================
//
// ============================================================================
//
// ============================================================================
//
#include <stdint.h>

#include <avr/io.h>
#include <avr/interrupt.h>
//#include <avr/eeprom.h>
#include <avr/pgmspace.h>
//
#include "servoturnout.h"
#include "servo.h"
#include "button.h"
#include "led.h"
#include "rom.h"
// 
// ============================================================================
volatile uint8_t TicCnt = 0;

uint8_t	eepromUpdateFlag = 0;		// Set when eeprom data is dirty
//
// ======================================================================================
// timer0_Init -- Initialize timer0 in CTC mode w/ interrupts enabled
//
// Operate the timer in CTC mode, the timer repeatedly counts from zero to OCR0A,
// and generates an TIMER0_COMPA interrupt at OCR0A. OCR0A and CS0x are selected
// to generate an approx 100Hz heartbeat interrupt.
//
// CS02:CS01:CS00 determines clock source and divisor
// 0b000: Clock off
// 0b001: ClkIO/1
// 0b010: ClkIO/8
// 0b011: ClkIO/64
// 0b100: ClkIO/256
// 0b101: ClkIO/1024
//
// Note: OCF0A is cleared by hardware when executing the interrupt handler
//
void timer0_Init( void )
{
	TIFR &= ~(OCF0A);					// Clear stale overflow interrupt
	TCCR0A = ((1<<WGM01) | (0<<WGM00));	// CTC Mode
	TCCR0B = ((0<<WGM02) | (1<<CS02) | (0<<CS01) | (1<<CS00));
	TCNT0 = 0;
	OCR0A = ((FCPU)/(1024L*SERVO_HZ));
//	OCR0A = 156;
	TIMSK |= (1<<OCIE0A);				// Interrupt when OCF0A is set
}
//
// ======================================================================================
// TIMER0_COMPA -- ISR for timer 0 compare match event
//
// This is the heartbeat ISR. Increment TicCnt and let the non-ISR code deal with
// everything.
//
ISR(TIMER0_COMPA_vect)
{
	++TicCnt;
}
//
// ======================================================================================
// timer1_Init -- Configure timer1 to generate 2 channel PWM, no interrupts
//
// Notes:
//	timer1 PWM channel 0 is on OC1A/PB3
//	timer1 PWM channel 1 is on OC1B/PB4
//	Use fast PWM WGM13:0 mode 14 (0b00001110) with the TOP value in ICR1
//
void timer1_Init( void )
{
	DDRB |= ((1<<PB3) | (1<<PB4));	// Set PWM pins for output
TIMSK &= ~(0xE8);				// Disable all timer 1 interrupts
	TCNT1 = 0;
	ICR1 = PWMTOP;
	OCR1A = SERVO_DEFAULT_CENTER;
	OCR1B = SERVO_DEFAULT_CENTER;
	TCCR1A = ((1<<COM1A1) | (0<<COM1A0) | (1<<COM1B1) | (0<<COM1B0) | (1<<WGM11) | (0<<WGM10));
	TCCR1B = ((1<<WGM13) | (1<<WGM12) | (0<<CS12) | (1<<CS11) | (0<<CS10));
	TCCR1C = 0;
}
//
// ============================================================================
// checkButtons -- check state of buttons, process any changes
//
static void checkButtons( void )
{
	for ( enum eButton idx=0; idx<(enum eButton)BUTTON_COUNT; ++idx ) {
		if (btnChanged( idx ) ) {
			switch( idx ) {
				case btnPlus:
					servoWiden();
					break;
				case btnMinus:
					servoNarrow();
					break;
				case btnServo1:
					servoToggle( servo1 );
					break;
				case btnServo2:
					servoToggle( servo2 );
					break;
			}
		}
	}

}
#ifdef LED_DEBUG
//
// ============================================================================
// testLeds
//
enum eLED cur = LED2;
uint16_t ledcnt = 10000;
//
void testLeds( void )
{

	if ( ++ledcnt > 100/6 ) {
		ledcnt = 0;
		switch ( cur ) {
			case LD1A:
				ledOff( LD1A );
				ledOn( LD1B );
				cur = LD1B;
				break;

			case LD1B:
				ledOff( LD1B );
				ledOn( LD2A );
				cur = LD2A;
				break;
				
			case LD2A:
				ledOff( LD2A );
				ledOn( LD2B );
				cur = LD2B;
				break;
				
			case LD2B:
				ledOff( LD2B );
				ledOn( LED1 );
				cur = LED1;
				break;
				
			case LED1:
				ledOff( LED1 );
				ledOn( LED2 );
				cur = LED2;
				break;
				
			case LED2:
				ledOff( LED2 );
				ledOn( LD1A );
				cur = LD1A;
				break;
		}
	}
}
#endif	// LED_DEBUG

#ifdef BTN_TEST

void btntest( void )
{
	uint8_t tmp;		// DEBUG

	tmp = PIND;
	if ( tmp & (1<<BTPLUS_PIN) ) {
		ledOn(LED1);
	}
	else {
		ledOff(LED1);
	}
	if ( tmp & (1<<BTMINUS_PIN) ) {
		ledOn(LED2);
	}
	else {
		ledOff(LED2);
	}
	if ( tmp & (1<<BTS1_PIN) ) {
		ledOn(LD1A);
	}
	else {
		ledOff(LD1A);
	}
	if ( tmp & (1<<BTS2_PIN) ) {
		ledOn(LD2A);
	}
	else {
		ledOff(LD2A);
	}
}

#endif	// BTN_TEST

//
// ============================================================================
// delayTic -- Delay one timer tic
//
void delayTic( void )
{
	uint8_t OldTic;

	OldTic = TicCnt;
	while ( OldTic == TicCnt )
		;
}
//
// ============================================================================
// delayCenti -- Wait for a number of 10ms intervals
//
void delayCenti( int centisecs )
{
	volatile int count;

	while ( centisecs>0 ) {
		for ( count=0; count<5000; ++count ) {
		}
		--centisecs;
	}
}
//
// ============================================================================
// selfTest -- Do a self test sequence
//
void selfTest( void )
{
	uint8_t iLED;
//	for ( uint8_t idx=0; idx<3; ++idx ) {
	while ( 1 ) {
		//for ( iLED=0; iLED<=LED_COUNT; ++iLED ) {
			ledOn( LD1A );
			ledOn( LD1B );
			ledOn( LD2A );
			ledOn( LD2B );
			ledOn( LED1 );
			delayTic();
			ledOff( LD1A );
			ledOff( LD1B );
			ledOff( LD2A );
			ledOff( LD2B );
			ledOff( LED1 );
			delayTic();
		//}
	}
}
//
// ============================================================================
//
int main(void)
{

	// Setup ==================================================================
	romServoDataInitialize();	// Initialize servo data from persistent storage
	timer0_Init();				// Configure timer0 to generate a heartbeat interrupt
	timer1_Init();				// Configure timer1 to generate PWM for the servo signal
	btnConfig();				// Configure button interface
	ledConfig();				// Configure LED interface

	selfTest();					// Do a self test sequence

	sei();

	// Loop ===================================================================
	while( 1 ) {
		if ( TicCnt ) {
			--TicCnt;
			btnHeartBeat();		// Do periodic service for the buttons

			// Debug steps
			// - verify servos
			// - verify eeprom functionality
			// - integrate subsystems
			
			// Check to see if a button has been pressed
			checkButtons();

			// Adjust servo positions
			servoMove();
		}
	}
}
//
// ============================================================================
//
