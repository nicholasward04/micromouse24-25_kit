# File for the translation of received maze data from wall bit representations
# to an array of strings. This array will be stored locally in a file and 
# subsequentally loaded into the maze display on the GUI. This will also allow
# for loading local maze files.

import os
from re import match
import mm_params as param

# Each row of the mouse should be 8 bytes * 32 rows = 256 bytes
# Each horizontal wall (North, South) should translate to ----
# Each vertical   wall (East, West)   should translate to  |
# Posts will be inserted in a grid pattern, where every wall section will be separated by a post, 'o'
# The mouse will be represented as different characters when facing: North: /\  South: \/  East: [)  West: (]
# Eg.   o----o----o----o 
#       |    |         | 
#       o    o----o    o                       [1101, 1011, 1100]
#       |    |    |    |  ----------------->   [0101, 1101, 0101]
#       o    o    o    o   translates from     [0110, 0010, 0110]
#       |              |
#       o----o----o----o

ROW_EVEN = "o    " * 15 + "o" + "\n"
ROW_ODD  = " " * 76 + "\n"

def update_maze_path():
    files = os.listdir(param.MAZE_DIR)
    prefix_match_files = []
    for file in files:
        if file.startswith(param.MAZE_PREFIX):
            prefix_match_files.append(file)
    param.MAZE_NUM = 0
    for file in prefix_match_files:
        param.MAZE_NUM = str(int(match(rf"{param.MAZE_PREFIX}(\d+)", file).group(1)) + 1)

    param.MAZE_FILE_PATH = param.MAZE_DIR + param.MAZE_PREFIX + str(param.MAZE_NUM) + param.MAZE_SUFFIX

def create_maze_file():
    with open(param.MAZE_FILE_PATH, 'w') as maze:
        for i in range(33):
            maze.write(ROW_EVEN) if i % 2 == 0 else maze.write(ROW_ODD)

def translate_maze(loc_maze, pos, dir):
    if int(param.MAZE_NUM) < 0:
        update_maze_path()
        create_maze_file()
    # Maze data received as [CURR_BYTE, NORTH_BYTE, EAST_BYTE, SOUTH_BYTE, WEST_BYTE]
    