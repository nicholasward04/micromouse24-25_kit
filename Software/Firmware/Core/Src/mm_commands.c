/*	mm_commands.c
 *
 *	File for implementation of reception and transmission of data and command packets
 *	through UART (and over Bluetooth)
 */
 
#include "mm_commands.h"
#include "mm_supplemental.h"
#include "mm_systick.h"

extern UART_HandleTypeDef huart1;

extern uint8_t debugMode;
extern uint8_t debugCounter;

extern mouse_state_t mouse_state;

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
			HAL_UART_Transmit_IT(&huart1, (uint8_t*)&(mouse_state.battery_voltage), sizeof(double));
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

void Create_Byte_Stream(uint8_t txData[PACKET_SIZE]) {
	bzero(txData, PACKET_SIZE);

	memcpy(txData, "Debug", 5);
	memcpy(txData + 5, &mouse_state.current_cell, sizeof(uint8_t));
	memcpy(txData + 6, &mouse_state.motor_L_RPM, sizeof(uint16_t));
	memcpy(txData + 8, &mouse_state.motor_R_RPM, sizeof(uint16_t));
	memcpy(txData + 10, &mouse_state.mouse_direction, sizeof(uint8_t));
	memset(txData + 11, ((mouse_state.mouse_position[0] << 4) | mouse_state.mouse_position[1]), sizeof(uint8_t));
	memcpy(txData + 12, &mouse_state.battery_voltage, sizeof(double));
	memcpy(txData + 20, &mouse_state.raw_FL, sizeof(uint16_t));
	memcpy(txData + 22, &mouse_state.raw_L, sizeof(uint16_t));
	memcpy(txData + 24, &mouse_state.raw_R, sizeof(uint16_t));
	memcpy(txData + 26, &mouse_state.raw_FR, sizeof(uint16_t));
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
