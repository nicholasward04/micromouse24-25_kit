# File for the translation of received maze data from wall bit representations
# to an array of strings. This array will be stored locally in a file and 
# subsequentally loaded into the maze display on the GUI. This will also allow
# for loading local maze files.

MAZE_FILE_NAME = "mm_maze_log.txt"

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

def translate_maze(byte_stream):
    print("translate maze entered")
    # Determine how received data is formatted, whether a stream of bytes or an array of streams of bytes