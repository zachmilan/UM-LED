# UM-LED

UM-LED is a personal embedded systems project I am currently developing. The main idea is to build a cool remote control LED display (in the shape of University of Michigan's "M") for my desk. This will be done by designing two PCB's - the remote and the display - and writing the firmware for each.

### Project Status 

As of now, the remote is fully assembled and I have verified functionality of most hardware components. Firmware is about halfway complete. Drivers for the accelerometer and haptic controller are done - I can reliably detect motion and can play haptic effects. Motion and button interrupts have been implemented and debounced. Capacitive touch input is functional via the TSC library, and an inactivity timer puts the remote to sleep when not in use. Battery charging and corresponding indicators (charging/complete) work as intended. 

Assembly was done using a stencil + hot-plate as well as soldering by hand for necessary parts. Luckily the first version of the board is sufficiently well designed and functions as intended after some debugging and rework. Photos of the assembled remote exist below: 

| Top View | Bottom View |
| :---: | :---: |
| <img src="https://github.com/zachmilan/UM-LED/blob/main/images/IMG_8023.jpg?raw=true" width="400" alt="UM-LED Remote Top View"> | <img src="https://github.com/zachmilan/UM-LED/blob/main/images/IMG_8024.jpg?raw=true" width="400" alt="UM-LED Remote PCB Detail"> |
| *The top contains the hardware and UI* | *The underside contains the battery and haptic motor (hot-glued)* |

### Remote Features:
* **Capacitive Touch Pads:** Used for easy brightness control.
* **Accelerometer:** Automatically wakes the remote when picked up.
* **Haptic Motor:** Provides clear feedback for user interactions.
* **USB-C Rechargeable:** Self explanatory, but easily charged with a USB-C cord and indicates charging status via lights.

---

### Next Steps:
Finishing the remote firmware is my current goal. Once complete, I will continue schematic design for the LED display, after which layout will commence. 

### More details
I wrote a full, more formal design proposal when beginning this project — see [UM-LED Design Proposal (PDF)](https://github.com/zachmilan/UM-LED/blob/main/docs/UM-LED%20Design%20Proposal%20(No%20Timeline).pdf). The main idea is still intact, though it may not reflect the full design once done (for example, the accelerometer was an afterthought and hence not included in the proposal).

Or, if you just want to get an idea of the general system architecture, see below: 

| Remote Architecture | Display Architecture |
| :---: | :---: |
| <img src="https://github.com/zachmilan/UM-LED/blob/main/images/Remote%20Block%20Diagram%20(2).png?raw=true" width="400" alt="UM-LED Remote Architecture"> | <img src="https://github.com/zachmilan/UM-LED/blob/main/images/Display%20Block%20Diagram%20(2).png?raw=true" width="500" alt="UM-LED Display Architecture"> |
| *Signal flow for the remote, including the MCU, sensors, and haptic feedback.* | *System architecture for the LED display, including MCU, sensors, and power regulation.* |
