# UM-LED

UM-LED is a personal embedded systems project that I am currently developing. The main idea is to build a cool remote control LED display for my desk. This will be done by designing two PCBs (the remote and the display) and writing the firmware for each.

As of now, the remote is fully assembled and I am nearing completion for the remote's firmware. Soldering was done using a hot-plate/stencil as well as by hand for through-hole parts. Luckily the first version of the board is working as intended (after a bit of rework and debugging).

### Key Features of the Remote:
* **Capacitive Touch Pads:** Used for intuitive brightness control.
* **Accelerometer:** Automatically wakes the remote when it is picked up.
* **Haptic Motor:** Provides clear tactile feedback for user interactions.

### Project Status & Next Steps:
My next step is to finish the remote firmware and then to design the schematic for the LED display (which will, in theory, be quicker than the remote).

I wrote a full, more formal design proposal when beginning this project—see [UM-LED Design Proposal (PDF)](UM-LED_Design_Proposal.pdf). The main idea is still intact, though it may not reflect the full design once done (for example, the accelerometer was an afterthought and hence is not included in the proposal).

---

### Assembled Remote Photos

| Top View | Bottom View |
| :---: | :---: |
| <img src="https://github.com/zachmilan/UM-LED/blob/main/images/IMG_8023.jpg?raw=true" width="400" alt="UM-LED Remote Top View"> | <img src="https://github.com/zachmilan/UM-LED/blob/main/images/IMG_8024.jpg?raw=true" width="400" alt="UM-LED Remote PCB Detail"> |
| *The top of the is simply most of the hardware and the UI* | *The underside hides the battery and haptic moter (both hot-glued)* |
