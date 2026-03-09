# UM-LED

UM-LED is a personal embedded systems project I am currently developing. The main idea is to build a cool remote control LED display (in the shape of the University of Michigan's "M") for my desk. This will be done by designing two PCB's - the remote and the display - and writing the firmware for each.

### Project Status 

March 9, 2026: The remote is complete and the firmware is in a consistent working state. Capacitive touch, BLE, haptics, motion detection, sleep timeout, charging and charging indicators, and low battery indicator all function as intended and have been integrated as a system. Changes to the remote's firmware will likely be made once the display is built and integrated, along with a few other polishing touches. Next step is the display PCB schematic. 

This is the first (and hopefully only) version of the remote PCB. Assembly was done using a stencil + hot-plate as well as hand soldering for necessary parts. Some rework and debugging was involved. 

Features, photos, and two short demo videos of the assembled remote exist below: 

### Remote Features:
* **Capacitive Touch Pads:** Used for easy brightness control.
* **Accelerometer:** Automatically wakes the remote when picked up.
* **Haptic Motor:** Provides clear tactile feedback for user interactions.
* **USB-C Rechargeable:** Self explanatory, but easily charged with a USB-C cord and indicates charging status via lights.
* **Low Battery Indication:** Red light informs user when remote needs to be charged.
  
| Top View | Bottom View |
| :---: | :---: |
| <img src="https://github.com/zachmilan/UM-LED/blob/main/images/IMG_8023.jpg?raw=true" width="400" alt="UM-LED Remote Top View"> | <img src="https://github.com/zachmilan/UM-LED/blob/main/images/IMG_8024.jpg?raw=true" width="400" alt="UM-LED Remote PCB Detail"> |
| *The top contains the hardware and UI* | *The underside contains the battery and haptic motor (hot-glued)* |

### Firmware Demos

<div align="center">

**Capacitive Touch Demo**

https://github.com/user-attachments/assets/bd866a0d-1efd-45fc-9129-0372a43028ab

*Each touch triggers an LED flash and haptic feedback (turn volume UP to hear haptics firing).*

**Remote Sleep Demo**

https://github.com/user-attachments/assets/c11abb02-e6e1-4325-9b7a-56cc4e109a6d

*The remote wakes upon movement, and falls back asleep after 10 seconds of inactivity. 

</div>

---


### More details
I wrote a full, more formal design proposal when beginning this project — see [UM-LED Design Proposal (PDF)](https://github.com/zachmilan/UM-LED/blob/main/docs/UM-LED%20Design%20Proposal%20(No%20Timeline).pdf). The main idea is still intact, though it may not reflect the full design once done (for example, the accelerometer was an afterthought and hence not included in the proposal).

Or, if you just want to get an idea of the general system architecture, see below: 

| Remote Architecture | Display Architecture |
| :---: | :---: |
| <img src="https://github.com/zachmilan/UM-LED/blob/main/images/Remote%20Block%20Diagram%20(2).png?raw=true" width="400" alt="UM-LED Remote Architecture"> | <img src="https://github.com/zachmilan/UM-LED/blob/main/images/Display%20Block%20Diagram%20(2).png?raw=true" width="500" alt="UM-LED Display Architecture"> |
| *Signal flow for the remote, including the MCU, sensors, and haptic feedback.* | *System architecture for the LED display, including MCU, sensors, and power regulation.* |
