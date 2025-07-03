# EVi Classic Front Panel Firmware

This project implements custom firmware for the EVi Classic front panel interface, built for the ATxmega256A3U microcontroller using Microchip Studio. Visit the Wiki for a more detailed breakdown.

## Features

- **Composite USB HID Device**:
  - HID Keyboard: Keypad sends keypresses as standard HID codes
  - HID Joystick: 12-button touch sliders mapped to 2D joystick movement
  - HID LED Device: Bi-directional LED control

- **Hardware Interfaces**:
  - 4×5 matrix keypad with dynamic scan and USB reporting
  - 12-position vertical and horizontal touch sliders interpreted as joystick axes
  - 8 controllable front-panel LEDs + 1 status LED

- **Real-Time GUI**:
  - Custom GUI panel built in Python to display and control LEDs, monitor HID sub-device operations, and verify proper device functionality.

## Running the Project

1. **Hardware:** Connect the EVi Classic Front Panel to your PC via USB.
2. **Build:** Compile the project using Microchip Studio for ATxmega256A3U.
3. **Flash:** Upload firmware to the device.
4. **Run GUI:** Use `EVi_FrontPanel_GUI.py` (requires `hid` and `tkinter`) to test & interact with the device.

## License

Proprietary / Internal Project. Do not distribute without permission.

---

© 2025 UniWest Inc. All rights reserved.