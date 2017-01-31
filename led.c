//
// ============================================================================
//
// led.c -- Provide LED functions to the servoturnout program
//
// ============================================================================
//
#include <stdint.h>
#include <avr/io.h>
//
#include "led.h"
//
// ============================================================================
// ledConfig -- Configure the LED interface
//
// Configure port pins as output and turn them off
//
void ledConfig( void )
{
	DDRB  |=  (1<<LD1A_PIN | 1<<LD1B_PIN | 1<<LED1_PIN);
	PORTB &= ~(1<<LD1A_PIN | 1<<LD1B_PIN | 1<<LED1_PIN);

	DDRD  |=  (1<<LD2A_PIN | 1<<LD2B_PIN | 1<<LED2_PIN);
	PORTD &= ~(1<<LD2A_PIN | 1<<LD2B_PIN | 1<<LED2_PIN);
}

// ============================================================================
// ledOff -- turn off the specified LED
//
void ledOff( enum eLED idx )
{
	switch( idx ) {
		case LD1A:
			PORTB &= ~(1<<LD1A_PIN);
			break;

		case LD1B:
			PORTB &= ~(1<<LD1B_PIN);
			break;

		case LD2A:
			PORTD &= ~(1<<LD2A_PIN);
			break;

		case LD2B:
			PORTD &= ~(1<<LD2B_PIN);
			break;

		case LED1:
			PORTB &= ~(1<<LED1_PIN);
			break;

		case LED2:
			PORTD &= ~(1<<LED2_PIN);
			break;

		default:
			break;
	}
}

// ============================================================================
// ledOn -- turn on the specified LED
//
void ledOn( enum eLED idx )
{
	switch( idx ) {
		case LD1A:
			PORTB |= (1<<LD1A_PIN);
			break;

		case LD1B:
			PORTB |= (1<<LD1B_PIN);
			break;

		case LD2A:
			PORTD |= (1<<LD2A_PIN);
			break;

		case LD2B:
			PORTD |= (1<<LD2B_PIN);
			break;

		case LED1:
			PORTB |= (1<<LED1_PIN);
			break;

		case LED2:
			PORTD |= (1<<LED2_PIN);
			break;

		default:
			break;
	}
}
