# File for the processing of incoming and outgoing commands.
# Will parse incoming data and format it for the GUI to display.
# Will translate outgoing commands to the specified formats.
# Will send and receive byte data

import mm_params as param
import serial
import struct
from re import sub

command_dict = {"SET_MODE": 0b0000, "HALT_RUN": 0b0001, "READ_BATT": 0b0010, "PULSE_BUZZ": 0b0011, "START_RUN": 0b0100, "PAIRED": 0b0101}

class c():
    SET_MODE    = 0b0000  # SET MODE: Debug or Normal mode -- In debug, constantly transmit data. In normal, do not transmit data unless requested.
    HALT_RUN    = 0b0001  # HALT RUN: Trap mouse operation. Instantly switch to debug mode.
    RESUME_RUN  = 0b0111  # RUME RUN: Release mouse operation. Stay in debug mode.
    READ_BATT   = 0b0010  # READ BATTERY: Read battery voltage.
    PULSE_BUZZ  = 0b0011  # PULSE BUZZER: Play a short noise from the buzzer.
    START_RUN   = 0b0100  # START RUN: Start a maze run. For testing purposes only.
    PAIRED      = 0b0101  # PAIRED: Receive OK response to verify received.

    EMPTY_DATA  = 0b0000  # For use in commands with no corresponding data to pad to 1-byte

def create_byte(top_nibble, bott_nibble):
    return (top_nibble << 4) | bott_nibble

def realign_packet(data):
    if len(data) == param.DEBUG_SIZE:
        return data[-1:] + data[:-1]
    return data

def flush_buffers():
    param.SOCKET.reset_input_buffer()
    param.SOCKET.reset_output_buffer()

def response_parse(response):
    response = str(response).find(param.ACK)
    if response:
        return "OK"
    else:
        return f"ERROR within response [{response}]"

def translate_command(command):
    if command == "FLUSH":
        return "FLUSH"

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
            bluetooth_send(create_byte(c.SET_MODE, c.EMPTY_DATA))
            flush_buffers()
            response = response_parse(bluetooth_receive(param.ACK_SIZE)) 
        case c.HALT_RUN:
            response = "stop OK"
        case c.RESUME_RUN:
            response = "resume OK"
        case c.READ_BATT:
            response = "batt OK"
        case c.PULSE_BUZZ:
            response = "pulse OK"
        case c.START_RUN:
            response = "start OK"
        case c.PAIRED:
            bluetooth_send(create_byte(c.PAIRED, c.EMPTY_DATA))
            response = response_parse(bluetooth_receive(param.ACK_SIZE))
        case "FLUSH": # Clientside only
            flush_buffers()
            response = "OK"
        case _:
            response = "Invalid Command"

    return response

def bluetooth_connect():
    try:
        param.PORT = "COM" + str(param.PORT)
        bl_sock = serial.Serial(param.PORT, baudrate=param.BAUD, timeout=param.TIMEOUT, parity=serial.PARITY_NONE)
        param.SOCKET = bl_sock
        flush_buffers()
        return "OK"
    except Exception as e:
        return "ERROR when attempting to pair"

def bluetooth_disconnect():
    param.SOCKET.close()

def bluetooth_send(data):
    try:
        param.SOCKET.write(bytes([data]))
    except Exception as e:
        return "ERROR when attempting to send data"

def bluetooth_receive(bytes_expected):
    try:
        data = param.SOCKET.read(bytes_expected)
        return data
    except Exception as e:
        return "ERROR when attempting to receive data"

def receive_and_update(ACK=None):
    if param.MODE:
        data = bluetooth_receive(param.DEBUG_SIZE)
        data = realign_packet(data)
        specifier = data[:5].decode('ascii')
        if specifier == "Debug":
            # Extract maze
            param.MAZE = []
            for i in range(16):
                row = list(data[5+i*16:5+(i+1)*16])
                param.MAZE.append(row)
            # Extract other data
            param.MOTOR_1_RPM = struct.unpack('>H', data[261:263])[0]
            param.MOTOR_2_RPM = struct.unpack('>H', data[263:265])[0]
            param.MOUSE_DIRECTION = param.DIRECTIONS[struct.unpack('B', bytes([data[265]]))[0]]
            param.MOUSE_POSITION = struct.unpack('B', bytes([data[266]]))[0]
            param.MOUSE_POSITION = [(param.MOUSE_POSITION >> 4) & 0x0F, param.MOUSE_POSITION & 0x0F]
            param.BATTERY_VOLTAGE = struct.unpack('<d', data[267:])[0]
