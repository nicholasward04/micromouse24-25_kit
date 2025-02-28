# File for the processing of incoming and outgoing commands.
# Will parse incoming data and format it for the GUI to display.
# Will translate outgoing commands to the specified formats.

import mm_params as param

command_dict = {"SET_MODE": 0b0000, "HALT_RUN": 0b0001, "READ_BATT": 0b0010, "PULSE_BUZZ": 0b0011, "START_RUN": 0b0100, "PAIRED": 0b0101}

class c():
    SET_MODE    = 0b0000  # SET MODE: Debug or Normal mode -- In debug, constantly transmit data. In normal, do not transmit data unless requested.
    HALT_RUN    = 0b0001  # HALT RUN: Trap mouse operation. Disable: control loop & motors, instantly switch to debug mode.
    READ_BATT   = 0b0010  # READ BATTERY: Read battery voltage.
    PULSE_BUZZ  = 0b0011  # PULSE BUZZER: Play a short noise from the buzzer.
    START_RUN   = 0b0100  # START RUN: Start a maze run. For testing purposes only.
    PAIRED      = 0b0101  # PAIRED: Send OK response to verify received.

    EMPTY_DATA  = 0b0000  # For use in commands with no corresponding data to pad to 1-byte

def translate_command(command):
    try:
        output = command_dict[command]
    except(KeyError):
        output = "invalid"

    return output

def command_line_parse(command):
    translation = command
    if type(command) is str:
        translation = translate_command(command.split()[0])

    response = ""
    match translation:
        case c.SET_MODE:
            response = "mode OK"            
        case c.HALT_RUN:
            response = "stop OK"
        case c.READ_BATT:
            response = "batt OK"
        case c.PULSE_BUZZ:
            response = "pulse OK"
        case c.START_RUN:
            response = "start OK"
        case c.PAIRED:
            response = "pair OK"
        case _:
            response = "Invalid Command"

    return response

def receive_and_update(ACK=None):
    if param.debugMode:
        print("Works as expected")
    if ACK:
        print("ACK Entered")
        # For commands that are not debug mode, will wait a period of time and if ACK received display in command history, otherwise timeout and display failed ACK
    # Called every 10ms.
    # Receive sent data, if any
    # Update variables in memory, in the case of a maze, translate it into a file