# Main file for bluetooth GUI.
# Using the pyserial library to implement bluetooth features.
# Using the tkinter library to implement user interface.

from mm_bluetooth_gui import *
from mm_maze_translation import *

def main():
    update_maze_path()
    create_maze_file()
    root.mainloop()

if __name__ == "__main__":
    main()