//
// button.c -- handle buttons for the servoturnout program
//
// ============================================================================
//
#include <stdint.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
//
#include "servoturnout.h"
#include "button.h"
#include "led.h"
//
// ============================================================================
//
button_t buttons[BUTTON_COUNT] = {
	{ 1<<BTPLUS_PIN,	BTNRELEASEDSTATE,	BTNRELEASEDSTATE,	0 },
	{ 1<<BTMINUS_PIN,	BTNRELEASEDSTATE,	BTNRELEASEDSTATE,	0 },
	{ 1<<BTS1_PIN,		BTNRELEASEDSTATE,	BTNRELEASEDSTATE,	0 },
	{ 1<<BTS2_PIN,		BTNRELEASEDSTATE,	BTNRELEASEDSTATE,	0 },
};
//
// ============================================================================
// btnPinRead -- Read the Port:Bit associated with a specific button
//
// return BTNPRESSED iff button is pressed
// return BTNRELEASED iff button is released,
//
static uint8_t btnPinRead( enum eButton btnId )
{
	if ( BTNPIN & buttons[btnId].btnPinMask ) {	// If PORT:PIN reads high the button is released
		return BTNRELEASED;
	}
	else {
		return BTNPRESSED;
	}
}
//
// ============================================================================
// btnConfig -- Configure the button interface
//
// Config each pin as input with pullup enabled and initialize the buttons array
// to match the button state
//
// TODO: set PUD bit in MCUCR
//
void btnConfig( void )
{
	uint8_t idx;

	for ( idx=0; idx<BUTTON_COUNT; ++idx ) {
		BTNDDR &= ~(buttons[idx].btnPinMask);	// Set port as input
		BTNPORT |= buttons[idx].btnPinMask;		// Activate pull-up
	}
}
//
// ============================================================================
// btnChanged -- Return 1 if button state has changed, 0 otherwise
//
// in	- btnId - is a Button enum
//
// NB: This function has the side effect of clearing btnChangeFlag
//
uint8_t btnChanged( enum eButton btnId )
{
	uint8_t retChanged = 0;

	if ( buttons[btnId].btnChangeFlag ) {
		retChanged = 1;
	}

	buttons[btnId].btnChangeFlag = 0;		// Clear button changed flag

	return retChanged;
}
//
// ============================================================================
// btnHeartBeat -- Do heartbeat processing for the buttons
//
void btnHeartBeat( void )
{
	enum eButton	idx;
	uint8_t			pinState;				// 0 or 1
	uint8_t			btnPin = BTNPIN;		// Read state of all the buttons

	for ( idx=0; idx<BUTTON_COUNT; ++idx ) {
		pinState = btnPinRead( idx );
		// Accumulate current pin state into recent button history
		buttons[idx].btnCurrentState = (buttons[idx].btnCurrentState<<2) | (pinState<<1)|pinState;
//		buttons[idx].btnCurrentState = (pinState==BTNPRESSED) ? BTNPRESSEDSTATE : BTNRELEASEDSTATE;
		if ( (buttons[idx].btnCurrentState == BTNPRESSEDSTATE) ||
					(buttons[idx].btnCurrentState == BTNRELEASEDSTATE) ) {
			if ( buttons[idx].btnCurrentState != buttons[idx].btnLastState ) {
				buttons[idx].btnLastState = buttons[idx].btnCurrentState;
				buttons[idx].btnChangeFlag = 1;
			}
		}
	}
//if ( buttons[btnMinus].btnCurrentState==BTNPRESSEDSTATE ) { ledOn(LED1); ledOff(LED2); }
//if ( buttons[btnMinus].btnCurrentState==BTNRELEASEDSTATE ) { ledOff(LED1); ledOn(LED2); }

}
//
// ============================================================================
