# File for the implementation of a GUI for sending commands through a terminal interface,
# receiving incoming data, displaying incoming data, and writing it to local files.

import tkinter as tk
import tkinter.font as tkFont
from time import time
from mm_bluetooth_parse import command_line_parse, c, receive_and_update, bluetooth_connect, bluetooth_disconnect, flush_buffers
from mm_maze_translation import translate_maze
import mm_params as param

root = tk.Tk()
root.title("Micromouse Communication GUI")

root.columnconfigure(0, weight=2)
root.columnconfigure(1, weight=1)
root.rowconfigure(0, weight=5)
root.rowconfigure(1, weight=1)

# Maze frame

def maze_frame_update(maze_sect, pos, dir):
    # Insert localized maze data into maze file
    translate_maze(maze_sect, pos, dir)
    # Print maze file line by line to maze frame
    maze_load.delete(0, tk.END)
    with open(param.MAZE_FILE_PATH, 'r') as maze:
        for row in maze:
            maze_load.insert(0, row.strip("\n")) # Figure out spacing after testing that this works

maze_frame = tk.Frame(root)
maze_frame.grid(row=0, column=0, sticky="nsew")

maze_frame.columnconfigure(0, weight=1)
maze_frame.rowconfigure(0, weight=1)

maze_load = tk.Listbox(maze_frame)
maze_load.grid(row=0, column=0, sticky="nsew", padx=(50, 25), pady=15)

# Terminal Frame

startTime = 0

def calculate_spaces(message):
    w_font = tkFont.nametofont(command_history.cget('font'))
    width = command_history.winfo_width()
    space_width = w_font.measure(" ")
    message_width = w_font.measure(message)

    num_spaces = int((width - message_width) / space_width)

    return num_spaces-3

def commandLine_Handler(event=None):
    text = command_line.get()
    if text:
        command_history_write(text, "OUTGOING")
        command_history_write(command_line_parse(text), "INCOMING")
        command_history_write(str(param.BATTERY_VOLTAGE), "INCOMING")
        command_line.delete(0, tk.END)

def command_history_write(write_data, direction):
    global startTime
    if not(startTime):
        startTime = time()

    elapsedTime = time() - startTime
    minutes = int(elapsedTime // 60)
    seconds = int(elapsedTime  % 60)
    elapsedTime = f"{minutes:02}:{seconds:02}"

    match direction:
        case "OUTGOING":
            message = elapsedTime + " OUTGOING - " + write_data
            command_history.insert(0, message)
        case "INCOMING":
            message = elapsedTime + " INCOMING - " + write_data
            spaces = calculate_spaces(message)

            command_history.insert(0, (spaces * " ") + message)


terminal_frame = tk.Frame(root)
terminal_frame.grid(row=0, column=1, sticky="nsew", padx=(0,50), pady=15)

terminal_frame.columnconfigure(0, weight=4)
terminal_frame.columnconfigure(1, weight=1)
terminal_frame.rowconfigure(0, weight=1)
terminal_frame.rowconfigure(1, weight=10)

command_line = tk.Entry(terminal_frame, relief=tk.SUNKEN, bd=2, state='disabled')
command_line.bind("<Return>", commandLine_Handler)
command_line.grid(row=0, column=0, sticky="ew")

command_line_send = tk.Button(terminal_frame, text="SEND", command=commandLine_Handler, state='disabled')
command_line_send.grid(row=0, column=1, sticky="ew")

command_history_scrollbar = tk.Scrollbar(terminal_frame, orient=tk.VERTICAL)
command_history = tk.Listbox(terminal_frame, yscrollcommand=command_history_scrollbar.set, relief=tk.SUNKEN, bd=2)
command_history_scrollbar.config(command=command_history.yview)
command_history.grid(row=1, column=0, columnspan=2, sticky="nsew")
command_history_scrollbar.grid(row=1, column=2, sticky="ns")

# Data Frame

def data_frame_update(motor_1, motor_2, batt, pos, dir):
    motor_1_rpm_value.delete(0, tk.END)
    motor_2_rpm_value.delete(0, tk.END)
    batt_voltage_value.delete(0, tk.END)
    mouse_position_value.delete(0, tk.END)
    mouse_direction_value.delete(0, tk.END)

    motor_1_rpm_value.insert(0, f"{motor_1}")
    motor_2_rpm_value.insert(0, f"{motor_2}")
    batt_voltage_value.insert(0, f"{batt}")
    mouse_position_value.insert(0, f"{pos[0]}, {pos[1]}")
    mouse_direction_value.insert(0, f"{dir}")

data_frame = tk.Frame(root, relief=tk.SUNKEN, bd=1)
data_frame.grid(row=1, column=0, sticky="nsew", padx=(50, 25), pady=(0, 15))

data_frame.columnconfigure((0,1,2,3,4,5), weight=0)
data_frame.rowconfigure((0,1), weight=0)

motor_1_rpm = tk.Label(data_frame, text="Motor 1 RPM:")
motor_1_rpm.grid(row=0, column=0, sticky="nw", padx=(10, 5), pady=(5, 0))
motor_1_rpm_value = tk.Listbox(data_frame, height=1, relief=tk.SUNKEN, bd=2)
motor_1_rpm_value.grid(row=0, column=1, stick="nw", pady=(5, 0))

motor_2_rpm = tk.Label(data_frame, text="Motor 2 RPM:")
motor_2_rpm.grid(row=1, column=0, sticky="nw", padx=(10, 5), pady=(5, 0))
motor_2_rpm_value = tk.Listbox(data_frame, height=1, relief=tk.SUNKEN, bd=2)
motor_2_rpm_value.grid(row=1, column=1, stick="nw", pady=(5, 0))


mouse_direction = tk.Label(data_frame, text="Direction:")
mouse_direction.grid(row=0, column=2, sticky="nw", padx=(10, 5), pady=(5, 0))
mouse_direction_value = tk.Listbox(data_frame, height=1, relief=tk.SUNKEN, bd=2)
mouse_direction_value.grid(row=0, column=3, stick="nw", pady=(5, 0))

mouse_position = tk.Label(data_frame, text="Position (X, Y):")
mouse_position.grid(row=1, column=2, sticky="nw", padx=(10, 5), pady=(5, 0))
mouse_position_value = tk.Listbox(data_frame, height=1, relief=tk.SUNKEN, bd=2)
mouse_position_value.grid(row=1, column=3, stick="nw", pady=(5, 0))

batt_voltage = tk.Label(data_frame, text="Battery (V):")
batt_voltage.grid(row=0, column=4, sticky="nw", padx=(10, 5), pady=(5, 0))
batt_voltage_value = tk.Listbox(data_frame, height=1, relief=tk.SUNKEN, bd=2)
batt_voltage_value.grid(row=0, column=5, stick="nw", pady=(5, 0))

# Quick Command Frame

def stopButton_Handler():
    command_history_write("SET_MODE", "OUTGOING")
    command_history_write(command_line_parse(c.HALT_RUN), "INCOMING")

def saveCommandLog_Handler():
    write_data = command_history.get(0, tk.END)
    with open(param.LOG_PATH, 'w') as log_file:
        if write_data:
            for line in reversed(write_data):
                log_file.write(line.strip(" ") + "\n")

def mouseMode_Handler():
    param.MODE = (param.MODE + 1) % 2
    command_history_write("SET_MODE", "OUTGOING")
    command_history_write(command_line_parse(c.SET_MODE), "INCOMING")
    toggleCommandLine(param.MODE) # TEST REMOVING THIS
    

def enableAll(err):
    if err == "OK":
        param.PAIRED = True
        mouse_mode_button.configure(state="normal")
        stop_operation_button.configure(state="normal")
        save_command_log_button.configure(state="normal")
        command_line_send.configure(state="normal")
        command_line.configure(state="normal")
        bl_disc_button.configure(state="normal")
        bl_pair_label.configure(text="    Paired    ")

        bl_pair_button.config(state='disabled')

def disableAll():
    param.MODE = 0
    mouse_mode_button.deselect()
    mouse_mode_button.configure(state="disable")
    stop_operation_button.configure(state="disable")
    save_command_log_button.configure(state="disable")
    command_line_send.configure(state="disable")
    command_line.configure(state="disable")
    bl_disc_button.configure(state="disable")

def toggleCommandLine(mode):
    if mode:
        command_line.config(state="disabled")
        command_line_send.config(state="disabled")
    else:
        command_line.config(state="normal")
        command_line_send.config(state="normal")

def bluetoothPairing_Handler():
    command_history_write("PAIRING REQUEST", "OUTGOING")
    temp_port = bl_pair_entry.get()
    if temp_port == f"COMPORT (def={param.DEF_PORT})":
        temp_port = param.DEF_PORT
    try:
        temp_port = int(temp_port)
        param.PORT = temp_port
    except ValueError:
        temp_port = f'"{temp_port}" is not a valid port number'
        command_history_write(temp_port, "INCOMING")
        return
    
    err = bluetooth_connect()
    command_history_write(err, "INCOMING")
    enableAll(err)

def bluetoothDisconnect_Handler():
    if (param.MODE == 1): # Ensure debug mode is disabled on mouse before disconnecting
        flush_buffers()
        command_history_write(command_line_parse("SET_MODE") + ": Disabled debug mode on mouse", "INCOMING")
    command_history_write("DISCONNECT", "OUTGOING")
    bluetooth_disconnect()

    disableAll()
    param.SOCKET = 0
    param.PAIRED = False
    bl_pair_label.config(text="Not Paired")
    bl_pair_button.config(state='normal')

def focusIn(event=None):
    if bl_pair_entry.get() == f"COMPORT (def={param.DEF_PORT})":
        bl_pair_entry.delete(0, tk.END)
        bl_pair_entry.config(fg="black")

def focusOut(event=None):
    if bl_pair_entry.get() == "":
        bl_pair_entry.insert(0, f"COMPORT (def={param.DEF_PORT})")
        bl_pair_entry.config(fg="gray")

quick_command_frame = tk.Frame(root)
quick_command_frame.grid(row=1, column=1, sticky="nsew", pady=(0, 25))

quick_command_frame.columnconfigure((0,1,2,3), weight=0)
quick_command_frame.rowconfigure((0,1,2), weight=0)
quick_command_frame.rowconfigure(3, weight=1)

mouse_mode_button = tk.Checkbutton(quick_command_frame, command=mouseMode_Handler, text="Debug Mode", state='disabled')
mouse_mode_button.grid(row=0, column=0, sticky="nw", pady=(0,0))

stop_operation_button = tk.Button(quick_command_frame, text="Stop Operation", command=stopButton_Handler, state='disabled')
stop_operation_button.grid(row=1, column=0, sticky="nw", pady=(5,0))

save_command_log_button = tk.Button(quick_command_frame, text="Save Log", command=saveCommandLog_Handler, state='disabled')
save_command_log_button.grid(row=2, column=0, sticky="nw", pady=(5,0))

bl_pair_button = tk.Button(quick_command_frame, text="Pair Device", command=bluetoothPairing_Handler)
bl_pair_button.grid(row=3, column=0, sticky="sw", pady=(5,0))

bl_pair_entry = tk.Entry(quick_command_frame, fg="gray")
bl_pair_entry.insert(0, f"COMPORT (def={param.PORT})")
bl_pair_entry.grid(row=3, column=1, sticky="sw", pady=(5,0))

bl_pair_label = tk.Label(quick_command_frame, text="Not Paired")
bl_pair_label.grid(row=3, column=2, sticky='sw', pady=(5,0), padx=(5,0))

bl_disc_button = tk.Button(quick_command_frame, text="Disconnect", state="disabled", command=bluetoothDisconnect_Handler)
bl_disc_button.grid(row=3, column=3, sticky='sw', pady=(5,0), padx=(5,0))

bl_pair_entry.bind("<FocusIn>", focusIn)
bl_pair_entry.bind("<FocusOut>", focusOut)

# Attempt update data every 50 milliseconds (can be changed in mm_params.py)

def update_display():
    if param.MODE:
        maze_frame_update(param.MAZE_SECTION, param.MOUSE_POSITION, param.MOUSE_DIRECTION)                                              # Update maze
        data_frame_update(param.MOTOR_1_RPM, param.MOTOR_2_RPM, param.BATTERY_VOLTAGE, param.MOUSE_POSITION, param.MOUSE_DIRECTION)     # Update parameter values

    root.after(param.WAIT_TIME, receive_and_update)
    root.after(param.WAIT_TIME, update_display)
    

root.after(param.WAIT_TIME, receive_and_update)       # Alternative to adding a thread constantly listening
root.after(param.WAIT_TIME, update_display)

def on_close():
    if param.SOCKET != 0:                             # If still paired, disconnect
        bluetoothDisconnect_Handler()
    root.quit()
root.protocol("WM_DELETE_WINDOW", on_close)