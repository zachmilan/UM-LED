# UM-LED

UM-LED is a personal embedded systems project I am currently developing. The main idea is to build a cool remote control LED display (in the shape of University of Michigan's "M") for my desk. This will be done by designing two PCB's - the remote and the display - and writing the firmware for each.

### Project Status 

As of now, the remote is fully assembled and I am nearing completion of the remote's firmware. Soldering was done using a stencil + hot-plate as well as by hand for necessary parts. Luckily the first version of the board is sufficiently well designed and functions as intended after some debugging and rework. Photos of the assembled remote exist below: 

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
I wrote a full, more formal design proposal when beginning this project — see [UM-LED Design Proposal (PDF)](UM-LED_Design_Proposal.pdf). The main idea is still intact, though it may not reflect the full design once done (for example, the accelerometer was an afterthought and hence is not included in the proposal).

Or, if you just want to get an idea of the general system architecture, see below: 

