# UM-LED

UM-LED is a remote-controlled LED display in the shape of the University of Michigan’s Block-M, paired with a custom handheld remote.  
Designed to resemble a polished consumer product, it features multiple display modes, adjustable brightness, haptic feedback, and rechargeability.

---

## System Overview

- **LED Display** – A Block-M array of individually controlled LEDs driven by dedicated LED drivers. The display supports patterns, animations, brightness control, auto-dimming with ambient light sensing, battery monitoring, and on-board power logging to microSD.  

- **Handheld Remote** – A BLE controller with a capacitive-touch brightness slider, tactile buttons for mode selection, haptic feedback, status indicators, and its own rechargeable battery.

---

## Concept Image

The rendering below illustrates the intended design aesthetic of UM-LED: a Block-M LED display paired with a compact handheld remote.  
This concept art communicates the target user experience and consumer-style finish, though the final prototype may differ in appearance.  

<p align="center">
  <img src="https://github.com/zachmilan/UM-LED/raw/main/images/final_proto_img.png" alt="UM-LED concept rendering" width="400"/>
</p>
<p align="center"><em>Figure 1: UM-LED concept rendering</em></p>

---

## Block Diagrams

UM-LED is implemented across two custom PCBs: one for the LED display and one for the handheld remote.  
The block diagrams below highlight the internal architecture of each board and the interactions between their microcontrollers, drivers, sensors, and supporting hardware.

<p align="center">
  <img src="https://github.com/zachmilan/UM-LED/raw/main/images/Display%20Block%20Diagram%20(2).png" alt="Display block diagram" width="600"/>
</p>
<p align="center"><em>Figure 2: Display block diagram</em></p>

<p align="center">
  <img src="https://github.com/zachmilan/UM-LED/raw/main/images/Remote%20Block%20Diagram%20(2).png" alt="Remote block diagram" width="600"/>
</p>
<p align="center"><em>Figure 3: Remote block diagram</em></p>

---

## Major Hardware Components

- **STM32F303RCT6 MCU** – Microcontroller used on both boards, providing control logic and capacitive-touch support through its TSC peripheral.  
- **IS31FL3248 LED Driver** – 48-channel constant-current driver on the display board for individual LED control.  
- **BM71 BLE Module** – Bluetooth Low Energy module enabling wireless communication between the display and remote.  
- **DRV2605L Haptic Driver** – Driver on the remote used to control the LRA and provide haptic feedback.  
- **MAX77715 USB-C Charger** – Power management IC on the display for handling battery charging over USB-C.  

---

## Development

UM-LED is a work in progress. Project materials will be added incrementally as designs are refined and milestones are reached.  

**Target completion date:** October 31, 2025.  

For the full design proposal, see [UM-LED Design Proposal (PDF)](docs/UM-LED%20Design%20Proposal.pdf).
