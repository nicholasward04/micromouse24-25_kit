# Global parameters

DIRECTIONS = ["NORTH", "EAST", "SOUTH", "WEST"]

MAZE_SECTION = []

MOTOR_1_RPM = 0
MOTOR_2_RPM = 0
MOUSE_DIRECTION = "NORTH"
MOUSE_POSITION = [0, 0]
BATTERY_VOLTAGE = 0

MAC_ADDRESS = "98:D3:21:F7:A1:F4"  # Use your bluetooth modules mac address here
DEF_PORT = 6                       # Default COMPORT, can change when pairing in GUI or change default here
PORT = DEF_PORT
SOCKET = 0
BAUD = 9600
TIMEOUT = 0.5

WAIT_TIME = 50

DEBUG_SIZE = 275
ACK_SIZE = 2
ACK = "ff"

PAIRED = False
MODE = 0                           # 0 NORMAL MODE, 1 DEBUG MODE
HALTED = False

LOG_PATH = "Logs/mm_command_log.txt"

MAZE_DIR = "Logs/Mazes/"
MAZE_PREFIX = "mm_maze_log_"
MAZE_NUM = -1
MAZE_SUFFIX = ".txt"
MAZE_FILE_PATH = MAZE_DIR + MAZE_PREFIX + str(MAZE_NUM) + MAZE_SUFFIX