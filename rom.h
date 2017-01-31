#ifndef __ROM_H_
#define __ROM_H_
//
void romServoDataInitialize( void );	// Initialize servo data from persistent storage
void romWrite( uint16_t * address, uint16_t value );	// Write a word to the backing store
//
#define ROM_MAX_ADDRESS				255
//
#define ROM_SIGNATURE				0x4545		// "EE"
#define ROM_MAJOR_VERSION			1
#define ROM_MINOR_VERSION			0
#define ROM_EEVERSION				((ROM_MAJOR_VERSION<<8)|(ROM_MINOR_VERSION))
//
#define ROM_ADDR_SIGNATURE			0x00
#define ROM_ADDR_EEVERSION			0x02
//
#define ROM_ADDR_SERVO1_MINPOS		0x04
#define ROM_ADDR_SERVO1_MAXPOS		0x06
#define ROM_ADDR_SERVO1_TARGETPOS	0x08
//
#define ROM_ADDR_SERVO2_MINPOS		0x0A
#define ROM_ADDR_SERVO2_MAXPOS		0x0C
#define ROM_ADDR_SERVO2_TARGETPOS	0x0E
//
#endif	// __ROM_H_
