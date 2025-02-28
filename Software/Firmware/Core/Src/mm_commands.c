/*	mm_commands.h
 *
 *	File for implementation of reception and transmission of data and command packets
 *	through UART (and over Bluetooth)
 */
 
#include "main.h"
#include "mm_commands.h"

// 8-bit wide commands [4-bit][4-bit] --> [Command Code][Other Data]
#define SET_MODE    0b0000  // SET MODE: Debug or Normal mode -- In debug, constantly transmit data. In normal, do not transmit data unless requested.
#define HALT_RUN    0b0001  // HALT RUN: Trap mouse operation. Disable: control loop & motors, instantly switch to debug mode.
#define READ_BATT   0b0010  // READ BATTERY: Read battery voltage.
#define PULSE_BUZZ  0b0011  // PULSE BUZZER: Play a short noise from the buzzer.
#define START_RUN   0b0100  // START RUN: Start a maze run. For testing purposes only.
#define PAIRED      0b0101  // PAIRED: Send OK response to verify received.

void Parse_Receive_Data(uint8_t rxBuff, uint8_t *txBuff) {
	uint8_t command = (rxBuff & 0xF0) >> 4;   // Isolate command code
	uint8_t data    = (rxBuff & 0x0F);        // Isolate other data
	switch(command) {
		case SET_MODE:
			break;
		case HALT_RUN:
			break;
		case READ_BATT:
			break;
		case PULSE_BUZZ:
			break;
		case START_RUN:
			break;
		case PAIRED:
			break;
		default:
			txBuff = (uint8_t*) "INVALID COMMAND";
			break;
	}
}

// Write functions to carry out the commands (some can't be defined as of now). Return an ERR code for each function.
// Store the ERR code in the transmit buffer.
