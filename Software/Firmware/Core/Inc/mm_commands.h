/*	mm_commands.h
 *
 *	File for implementation of reception and transmission of data and command packets
 *	through UART (and over Bluetooth)
 */

#include "main.h"

#ifndef INC_MM_COMMANDS_H_
#define INC_MM_COMMANDS_H_

#define PACKET_SIZE 28

void Parse_Receive_Data(uint8_t rxBuff);
void Debug_Packet_Send();
void Create_Byte_Stream(uint8_t txData[PACKET_SIZE]);
void UART_Receive_Callback(UART_HandleTypeDef *huart);

#endif /* INC_MM_COMMANDS_H_ */
