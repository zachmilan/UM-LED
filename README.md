# UM-LED

UM-LED is a remote-controlled LED display in the shape of the University of Michigan’s Block-M, paired with a custom handheld remote.  
Designed to resemble a polished consumer product, it includes multiple display modes, adjustable brightness, haptic feedback, and rechargeable batteries.

---

## System Overview

- **LED Display** – A Block-M array of individually controlled through-hole LEDs driven by dedicated LED drivers. It supports patterns, animations, brightness control, auto-dimming with ambient light sensing, battery monitoring, and on-board power logging to microSD.  

- **Handheld Remote** – A BLE controller featuring a capacitive-touch brightness slider, tactile buttons for mode selection, haptic feedback, status indicators, and its own rechargeable battery.

---

## Concept Image

An early prototype illustrates UM-LED’s consumer-style design: a Block-M LED display paired with a compact remote. Both units prioritize simplicity, feedback, and intuitive controls.  

<p align="center">
  <img src="https://github.com/zachmilan/UM-LED/raw/main/images/final_proto_img.png" alt="UM-LED concept / prototype image" width="400"/>
</p>
<p align="center"><em>Figure 1: UM-LED concept / prototype image</em></p>

---

## Block Diagrams

The project is built around two custom PCBs - one for the LED display and one for the handheld remote. The block diagrams below illustrate how each board integrates its microcontroller, peripherals, and supporting components.

<p align="center">
  <img src="https://github.com/zachmilan/UM-LED/raw/main/images/Display%20Block%20Diagram%20(2).png" alt="Display block diagram" width="500"/>
</p>
<p align="center"><em>Figure 2: Display block diagram</em></p>

<p align="center">
  <img src="https://github.com/zachmilan/UM-LED/raw/main/images/Remote%20Block%20Diagram%20(2).png" alt="Remote block diagram" width="500"/>
</p>
<p align="center"><em>Figure 3: Remote block diagram</em></p>

---

## Key Components

- **STM32F303RCT6 MCU** – Microcontroller for both boards, chosen for its high pin count and TSC peripheral.  
- **IS31FL3248 LED Driver** – 48-channel constant-current driver for the display.  
- **BM71 BLE Module** – Provides Bluetooth Low Energy communication between remote and display.  
- **DRV2605L Haptic Driver** – Drives the LRA for haptic feedback in the remote.  
- **MAX77715 USB-C Charger** – Manages charging for the display’s battery.  

---

## Development

This repository contains the complete project: hardware design files, embedded firmware, and supporting documentation.  
Files will be added as development progresses.  

**Estimated completion date:** October 31, 2025.  

For the full design proposal, see [UM-LED Design Proposal (PDF)](docs/UM-LED%20Design%20Proposal.pdf).
