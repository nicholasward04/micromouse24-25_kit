/*	mm_commands.h
 *
 *	File for implementation of reception and transmission of data and command packets
 *	through UART (and over Bluetooth)
 */

#ifndef INC_MM_COMMANDS_H_
#define INC_MM_COMMANDS_H_

void Parse_Receive_Data(uint8_t rxBuff, uint8_t *txBuff);

#endif /* INC_MM_COMMANDS_H_ */
