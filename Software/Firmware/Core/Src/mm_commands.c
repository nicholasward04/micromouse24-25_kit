/*	mm_commands.c
 *
 *	File for implementation of reception and transmission of data and command packets
 *	through UART (and over Bluetooth)
 */
 
#include "mm_commands.h"
#include "mm_supplemental.h"

extern UART_HandleTypeDef huart1;

extern uint8_t debugMode;
extern uint8_t debugCounter;

extern double battery_voltage;
extern uint16_t raw_FL;
extern uint16_t raw_L;
extern uint16_t raw_R;
extern uint16_t raw_FR;

uint8_t txData = 0xFF;
uint8_t rxData = 0;

uint8_t txDebug[PACKET_SIZE];
uint8_t HALTED = 0;

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
	uint8_t data    = (rxBuff & 0x0F);        // Isolate other data (not used currently)
	(void) data;                              // Temporary to remove warning for unused variable

	switch(command) {
		case SET_MODE:
			debugMode = (debugMode + 1) % 2;
			debugCounter = 0;
			break;
		case HALT_RUN:
			HALTED = 1;
			while (HALTED);
			break;
		case RESUME_RUN:
			HALTED = 0;
			break;
		case READ_BATT:
			HAL_UART_Transmit_IT(&huart1, (uint8_t*)&battery_voltage, sizeof(double));
			break;
		case PULSE_BUZZ:
			Pulse_Buzzer(100);
			break;
		case START_RUN:
			// SET RUN START VAR TO TRUE
			break;
		case PAIRED:
			break;
		default:
			break;
	}
}

uint8_t cell = 0b1101;
uint16_t motor_1_rpm = 2048;
uint16_t motor_2_rpm = 2048;
uint8_t direction = 1;         // East
uint8_t position = 0b00010010; // First 4 bits is x position, last 4 bits is y position (1, 2)

void Create_Byte_Stream(uint8_t txData[PACKET_SIZE]) {
	bzero(txData, PACKET_SIZE);

	memcpy(txData, "Debug", 5);
	memcpy(txData + 5, &cell, sizeof(uint8_t));
	memcpy(txData + 6, &motor_1_rpm, sizeof(uint16_t));
	memcpy(txData + 8, &motor_2_rpm, sizeof(uint16_t));
	memcpy(txData + 10, &direction, sizeof(uint8_t));
	memcpy(txData + 11, &position, sizeof(uint8_t));
	memcpy(txData + 12, &battery_voltage, sizeof(double));
	memcpy(txData + 20, &raw_FL, sizeof(uint16_t));
	memcpy(txData + 22, &raw_L, sizeof(uint16_t));
	memcpy(txData + 24, &raw_R, sizeof(uint16_t));
	memcpy(txData + 26, &raw_FR, sizeof(uint16_t));
}

// When mouse in debug mode transmit above data.
//  Also transmit start identifier "Debug". This totals to 28 bytes of data transmitted every 50 ms.
void Debug_Packet_Send() {
	Create_Byte_Stream(txDebug);                                  // Create byte stream
	HAL_UART_Transmit_IT(&huart1, txDebug, sizeof(txDebug));      // Transmit data
}

void UART_Receive_Callback(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART1) {
		Parse_Receive_Data(rxData);
		HAL_UART_Transmit_IT(&huart1, &txData, sizeof(txData));      // Transmit response ACK (0xFF)                          // Parse incoming command
		HAL_UART_Receive_IT(&huart1, &rxData, sizeof(rxData));       // Receive incoming command
	}
}
