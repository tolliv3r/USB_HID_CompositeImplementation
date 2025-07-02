import hid
import time
import sys

# adjust to your VID/PID and LED interface number
VID, PID = 0x03EB, 0x2133
LED_IFACE = 2

def find_and_open():
    for d in hid.enumerate(VID, PID):
        if d['interface_number'] == LED_IFACE:
            dev = hid.device()
            dev.open_path(d['path'])
            dev.set_nonblocking(True)
            return dev
    return None

def main():
    dev = find_and_open()
    if not dev:
        print(f"No device with VID=0x{VID:04X}, PID=0x{PID:04X}, IF={LED_IFACE}")
        sys.exit(1)

    print("Opened LED interface, listening for 7-byte IN reports… (Ctrl-C to quit)\n")
    try:
        while True:
            # dev.write([0x00, 0x00])

            rpt = dev.read(7)
            if rpt and len(rpt) >= 7:
                # unpack report bytes
                led_lo, led_hi, keys_lo, keys_hi, joy_lo, joy_mid, joy_hi = rpt[:7]

                # reconstruct masks
                key_mask = keys_lo | (keys_hi << 8)
                joy_mask = joy_lo | (joy_mid << 8) | (joy_hi << 16)

                # convert led mask to bitmap
                led_bits = [(led_lo >> i) & 1 for i in range(8)]

                # convert keypad mask to bitmap
                key_bits = [(key_mask >> i) & 1 for i in range(9)]

                # split vertical/horizontal 12-bit masks
                vert_mask = joy_mask & 0x0FFF
                hori_mask = (joy_mask >> 12) & 0x0FFF

                # convert joystick slider masks to bitmaps
                vert_bits = [(vert_mask >> i) & 1 for i in range(12)]
                hori_bits = [(hori_mask >> i) & 1 for i in range(12)]

                # print
                print(f"Status LED:      [{led_hi & 0x01}]")
                print(f"LED bitmap:      {led_bits}\n")
                print(f"Keypad bitmap:   {key_bits}\n")
                print(f"V Slider bitmap: {vert_bits}")
                print(f"H Slider bitmap: {hori_bits}")
                print("-" * 40)

            time.sleep(1)

    except KeyboardInterrupt:
        pass
    finally:
        dev.close()

if __name__ == "__main__":
    main()
