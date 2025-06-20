# EVi Classic Front Panel Firmware

This project implements custom firmware for the EVi Classic front panel interface (P/N 76319), built for the ATxmega256A3U microcontroller using Microchip Studio.

## Features

- **Composite USB HID Device**:
  - HID Keyboard: Keypad sends keypresses as standard HID codes
  - HID Joystick: 12-button touch sliders mapped to 2D joystick movement
  - HID LED Device: Bi-directional LED control and status feedback

- **Hardware Interfaces**:
  - 4×5 matrix keypad with dynamic scan and USB reporting
  - 12-position vertical and horizontal touch sliders interpreted as joystick axes
  - 8 controllable front-panel LEDs + 1 status LED
  - Configurable startup LED sequences and test modes

- **Real-Time GUI (Python)**:
  - HID-compatible Python `tkinter` GUI to display and control LED states

## Getting Started

1. **Hardware**: Connect the EVi Classic Front Panel to your PC via USB.
2. **Build**: Compile the project using Microchip Studio for ATxmega256A3U.
3. **Flash**: Upload firmware to the device.
4. **Run GUI**: Use `led_gui.py` (requires `hid` and `tkinter`) to interact with the LED interface.

## Dependencies

- [Microchip ASF](https://www.microchip.com/en-us/tools-resources/develop/libraries/advanced-software-framework)
- Python 3.6+ with:
  - `hid`
  - `tkinter`

## License

Proprietary / Internal Project. Do not distribute without permission.
