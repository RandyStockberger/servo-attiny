
#ifndef _BUTTON_H_
#define _BUTTON_H_
//
// ============================================================================
//
// button.h -- button defines and declarations for the servoturnout program
//
// ============================================================================
//
// ATtinyx313 button PORT/PIN assignments:
//	BTPLUS	PD0
//	BTMINUS	PD1
//	BTS1	PD2
//	BTS2	PD3
//
// ============================================================================
// Button data structures and data
//
typedef struct {
	uint8_t		btnPinMask;		// Which port pin is associated with this button
	uint8_t		btnLastState;	// Last stable state of this button
	uint8_t		btnCurrentState;// Current, perhaps unstable, state of this button
	uint8_t		btnChangeFlag;	// Flagged as changed when btnCurrentState is stable
								// and doesn't match btnLastState
} button_t;
//
// ============================================================================
// Button defines
//
// The buttons this program knows about
enum eButton { btnPlus, btnMinus, btnServo1, btnServo2 };
//
#define BUTTON_COUNT		4
//
#define BTNPRESSED			0		// Pin state when button is pressed
#define BTNRELEASED			1		// Pin state when button is not pressed
//
#define BTNPRESSEDSTATE		0x00	// Debounced button state when button is pressed
#define BTNRELEASEDSTATE	0xFF	// Debounced button state when button is released
//
#define BTNPORT				PORTD
#define BTNDDR				DDRD
#define BTNPIN				PIND
//
#define BTPLUS_PIN			0
#define BTMINUS_PIN			1
#define BTS1_PIN			2
#define BTS2_PIN			3
//
// ============================================================================
// Button interface functions
void btnConfig( void );						// Initialize ports to support the buttons
uint8_t btnChanged( enum eButton btnId );	// Return 1 if a button state has changed
void btnHeartBeat( void );					// Update button data structure based on hardware state
//
// ============================================================================
//
#endif // _BUTTON_H_
