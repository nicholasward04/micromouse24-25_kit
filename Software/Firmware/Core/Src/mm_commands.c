/*	mm_commands.h
 *
 *	File for implementation of reception and transmission of data and command packets
 *	through UART (and over Bluetooth)
 */
 
#include "main.h"
#include "mm_commands.h"

extern huart1;

extern debugMode;
extern debugCounter;

uint8_t txDebug[275];

// 8-bit wide commands [4-bit][4-bit] --> [Command Code][Other Data]
#define SET_MODE    0b0000  // SET MODE: Debug or Normal mode -- In debug, constantly transmit data. In normal, do not transmit data unless requested.
#define HALT_RUN    0b0001  // HALT RUN: Trap mouse operation. Disable: control loop & motors, instantly switch to debug mode.
#define RESUME_RUN  0b0111  // RESUME RUN: Release mouse operation. Stay in debug mode.
#define READ_BATT   0b0010  // READ BATTERY: Read battery voltage.
#define PULSE_BUZZ  0b0011  // PULSE BUZZER: Play a short noise from the buzzer.
#define START_RUN   0b0100  // START RUN: Start a maze run. For testing purposes only.
#define PAIRED      0b0101  // PAIRED: Send OK response to verify received.

void Parse_Receive_Data(uint8_t rxBuff) {
	uint8_t command = (rxBuff & 0xF0) >> 4;   // Isolate command code
	uint8_t data    = (rxBuff & 0x0F);        // Isolate other data

	switch(command) {
		case SET_MODE:
			debugMode = (debugMode + 1) % 2;
			debugCounter = 0;
			break;
		case HALT_RUN:
			break;
		case RESUME_RUN:
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
			break;
	}
}

uint8_t maze[3][3] = {{0b1101, 0b1011, 0b1100}, {0b0101, 0b1101, 0b0101}, {0b0110, 0b0010, 0b0110}};
uint16_t motor_1_rpm = 2048;
uint16_t motor_2_rpm = 2048;
uint8_t direction = 1;         // East
uint8_t position = 0b00010010; // First 4 bits is x position, last 4 bits is y position (1, 2)
double battery_volt = 8.43;

void Create_Byte_Stream(uint8_t txData[275]) {
	bzero(txData, 275);

	memcpy(txData, "Debug", 5);
	memcpy(txData + 5, maze, 9);                    // Once confirmed to work, change to memcpy actual 16x16 maze array
	memset(txData + 14, 0, 256 - 9);
	txData[261] = (uint8_t)(motor_1_rpm >> 8);      // High byte
	txData[262] = (uint8_t)(motor_1_rpm & 0xFF);    // Low byte
    txData[263] = (uint8_t)(motor_2_rpm >> 8);      // High byte
    txData[264] = (uint8_t)(motor_2_rpm & 0xFF);    // Low byte
    txData[265] = direction;
    txData[266] = position;
    memcpy(&txData[267], &battery_volt, sizeof(battery_volt));
}

// When mouse in debug mode, transmit maze (256 bytes), motor 1 rpm (2 bytes), motor 2 rpm (2 bytes), direction (1 byte), position (1 byte), and battery voltage (8 bytes).
// Also transmit start identifier "Debug". This totals to 275 bytes of data transmitted every 100 ms.
void Debug_Packet_Send() {
	Create_Byte_Stream(txDebug);

	HAL_UART_Transmit_IT(&huart1, txDebug, sizeof(txDebug));      // Transmit data
}

// Write functions to carry out the commands (some can't be defined as of now). Return an ERR code for each function.
// Store the ERR code in the transmit buffer.
