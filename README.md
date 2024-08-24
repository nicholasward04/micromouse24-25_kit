The following repository will contain the work I've done as lead of the UCI Micromouse program. Some of the files included are inaccessible shortcuts and are generally administrative in nature. If you would like access please contact me at 818-233-1594 or nnward2004@icloud.com. 

The PCB is iterative and has had 2 other collaborators work on it in previous years. The PCB design was done in KiCAD 7.0 and the STM32 will be programmed using the STM32CubeIDE. I've changed the motor driver to something more capable than the L293D, added a circuit to protect the microcontroller when both the battery and STLINK are being used in full, and added an extra button. The PCB is 4-layer board with 3 GND planes and 1 power plane (split into +5V and +3V3 sections). I rerouted the entire board to fit in the new circuitry while maintaining the legacy aspects of the schematic. 
There are 2 versions of the design, one using an STM32 bluepill and one using solely the STM32F103C8T6 chip. This is to provide the option for teams that are part of the program to route their own boards using either one. 

Below is the relevant schematic.
![image](https://github.com/user-attachments/assets/57274af3-9ff8-443b-be0d-c0a3542038d5)

Version using STM32 Bluepill front and back. The middle 2 layers of the board are mostly empty asides from a few traces and a GND and power plane, and therefore will not be included by image.

<img src="https://github.com/user-attachments/assets/7a1101fa-6da8-4797-8f15-576cd310add3" width="500">  <img src="https://github.com/user-attachments/assets/772ac968-2068-4fbf-b332-b3c57b703e93" width="500">

Version using STM32F103C8T6 chip front and back. Same as previous in terms of middle 2 layers. 

<img src="https://github.com/user-attachments/assets/0bcf6cd8-7790-47f7-8646-606f38799931" width="500">  <img src="https://github.com/user-attachments/assets/06eaa6ac-2706-48ad-be41-6625ef521af0" width="500">

Both designs are almost completely identical besides a power LED and a few capacitors. 

[include bare PCBs here]

[include completed PCBs here once soldered]

The software must still be worked on, but will be a revamped/redone version of my previous years mouse software, which can be found here <- insert link. As I work on it, I will push changes to this repository (micromouse24-25_kit). 
