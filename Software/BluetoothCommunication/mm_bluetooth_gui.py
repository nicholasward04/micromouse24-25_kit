# File for the implementation of a GUI for sending commands through a terminal interface,
# receiving incoming data, displaying incoming data, and writing it to local files.
#

import tkinter as tk

root = tk.Tk()
root.title("Micromouse Communication GUI")

root.columnconfigure(0, weight=1)
root.rowconfigure(0, weight=1)

# Maze frame

maze_frame = tk.Frame(root)
maze_frame.grid(row=0, column=0, sticky="nsew")

maze_frame.columnconfigure(0, weight=1)
maze_frame.rowconfigure(0, weight=1)

maze_load = tk.Listbox(maze_frame)
maze_load.grid()

# Terminal Frame

def commandLine_Handler(event=None):
    text = command_line.get()
    if text:
        command_history.insert(0, "Out: " + text)
        command_line.delete(0, tk.END)

terminal_frame = tk.Frame(root)
terminal_frame.grid(row=0, column=1, sticky="nsew")

terminal_frame.columnconfigure(0, weight=1)
terminal_frame.rowconfigure(0, weight=1)

command_line = tk.Entry(terminal_frame)
command_line.bind("<Return>", commandLine_Handler)
command_line.grid(row=0, column=0, sticky="nsew")

command_line_send = tk.Button(terminal_frame, text="SEND", command=commandLine_Handler)
command_line_send.grid(row=0, column=1, sticky="nsew")

command_history = tk.Listbox(terminal_frame)
command_history.grid(row=1, column=0, columnspan=2, sticky="nsew")

# Data Frame

data_frame = tk.Frame(root)
data_frame.grid(row=1, column=0, sticky="nsew")

data_frame.columnconfigure(0, weight=1)
data_frame.rowconfigure(0, weight=1)

motor_1_pwm = tk.Label(data_frame)
motor_1_pwm.grid(row=0, column=0, sticky="w")

motor_2_pwm = tk.Label(data_frame)
motor_2_pwm.grid(row=1, column=0, sticky="w")

batt_voltage = tk.Label(data_frame)
batt_voltage.grid(row=0, column=1, sticky="w")

mouse_position = tk.Label(data_frame)
mouse_position.grid(row=1, column=1, sticky="w")

# Quick Command Frame

debugMode = 0

def stopButton_Handler():
    print("Call stop operation function here")

def mouseMode_Handler():
    global debugMode
    debugMode = (debugMode + 1) % 2
    print("Call mode switch function here")

quick_command_frame = tk.Frame(root)
quick_command_frame.grid(row=1, column=1, sticky="nsew")

quick_command_frame.columnconfigure(0, weight=1)
quick_command_frame.rowconfigure(0, weight=1)

mouse_mode_button = tk.Checkbutton(quick_command_frame, command=mouseMode_Handler)
mouse_mode_button.grid(row=0, column=0, sticky="w")

debug_label = tk.Label(quick_command_frame, text="Debug Mode")
debug_label.grid(row=0, column=1, sticky="w")

stop_operation_button = tk.Button(quick_command_frame, text="Stop Operation", command=stopButton_Handler)
stop_operation_button.grid(row=1, column=0, sticky="ew")

