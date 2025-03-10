# File for the processing of incoming and outgoing commands.
# Will parse incoming data and format it for the GUI to display.
# Will translate outgoing commands to the specified formats.
# Will send and receive byte data

import mm_params as param
import serial
import struct

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

# Bluetooth functions

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
    
# Command functions

def set_mode():
    bluetooth_send(create_byte(c.SET_MODE, c.EMPTY_DATA))
    response = response_parse(bluetooth_receive(param.ACK_SIZE))

    return response

def halt_run():
    bluetooth_send(create_byte(c.HALT_RUN, c.EMPTY_DATA))
    response = response_parse(bluetooth_receive(param.ACK_SIZE))
    if response == "OK":
        param.HALTED = True

    return response

def resume_run():
    bluetooth_send(create_byte(c.RESUME_RUN, c.EMPTY_DATA))
    response = response_parse(bluetooth_receive(param.ACK_SIZE))
    if response == "OK":
        param.HALTED = False

    return response

def read_batt():
    bluetooth_send(create_byte(c.READ_BATT, c.EMPTY_DATA))
    flush_buffers()
    ack_data = bluetooth_receive(param.ACK_SIZE + 8) # Byte size of double=8
    response = response_parse(ack_data)
    param.BATTERY_VOLTAGE = struct.unpack('<d', ack_data[2:])[0]
    from mm_bluetooth_gui import command_history_write # Delayed import for circular import
    command_history_write(command_line_parse(str(param.BATTERY_VOLTAGE) + "V"), "INCOMING")

    return response

def pulse_buzz():
    bluetooth_send(create_byte(c.PULSE_BUZZ, c.EMPTY_DATA))
    response = response_parse(bluetooth_receive(param.ACK_SIZE))

    return response

def start_run():
    bluetooth_send(create_byte(c.START_RUN, c.EMPTY_DATA))
    response = response_parse(bluetooth_receive(param.ACK_SIZE))

    return response

def paired():
    bluetooth_send(create_byte(c.PAIRED, c.EMPTY_DATA))
    response = response_parse(bluetooth_receive(param.ACK_SIZE))

    if response_parse(response) != "OK":
        from mm_bluetooth_gui import bluetoothDisconnect_Handler # Delayed import for circular import
        bluetoothDisconnect_Handler()

    return response

def flush():
    flush_buffers()
    
    return "OK"
 
def command_line_parse(command):
    translation = command
    if type(command) is str:
        translation = translate_command(command.split()[0])

    response = ""
    match translation:
        case c.SET_MODE:
            response = set_mode()
        case c.HALT_RUN:
            response = halt_run()
        case c.RESUME_RUN:
            response = resume_run()
        case c.READ_BATT:
            response = read_batt()
        case c.PULSE_BUZZ:
            response = pulse_buzz()
        case c.START_RUN:
            response = start_run()
        case c.PAIRED:
            response = paired()
        case "FLUSH": # Clientside only
            response = flush()
        case _:
            response = "Invalid Command"

    return response

def realign_packet(data):
    for i in range(6):
        debug_index = data.find(b"Debug")
        if debug_index != -1:
            return data[debug_index:] + data[:debug_index]
        else:
            data = data[-1:] + data[:-1]
    return data

def receive_and_update():
    if param.MODE:
        data = bluetooth_receive(param.DEBUG_SIZE)
        data = realign_packet(data)
        flush()
        try:
            specifier = data[:5].decode('ascii')
        except Exception as e:
            return
        if specifier == "Debug":
            try:
                param.CELL = (struct.unpack('B', bytes([data[5]]))[0])
                param.MOTOR_1_RPM = struct.unpack('>H', data[6:8])[0]
                param.MOTOR_2_RPM = struct.unpack('>H', data[8:10])[0]
                param.MOUSE_DIRECTION = param.DIRECTIONS[struct.unpack('B', bytes([data[10]]))[0]]
                param.MOUSE_POSITION = struct.unpack('B', bytes([data[11]]))[0]
                param.MOUSE_POSITION = [(param.MOUSE_POSITION >> 4) & 0x0F, param.MOUSE_POSITION & 0x0F]
                param.BATTERY_VOLTAGE = struct.unpack('<d', data[12:20])[0]
                param.RAW_FL = struct.unpack('>H', data[20:22])[0]
                param.RAW_L = struct.unpack('>H', data[22:24])[0]
                param.RAW_R = struct.unpack('>H', data[24:26])[0]
                param.RAW_FR = struct.unpack('>H', data[26:])[0]
            except Exception as e:
                return
