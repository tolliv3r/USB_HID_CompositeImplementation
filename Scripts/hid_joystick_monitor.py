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
            dev.set_nonblocking(False)
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
            dev.write([0x00, 0x00])

            rpt = dev.read(7)
            if rpt and len(rpt) >= 7:
                # unpack report bytes
                led_lo, led_hi, keys_lo, keys_hi, joy_lo, joy_mid, joy_hi = rpt[:7]

                # reconstruct masks
                led_mask = led_lo | (led_hi << 8)
                key_mask = keys_lo | (keys_hi << 8)
                joy_mask = joy_lo | (joy_mid << 8) | (joy_hi << 16)

                # make led bit mask
                led_bits = [(led_mask>>i)&1 for i in range(8)]

                # split vertical/horizontal 12-bit maps
                vert_mask = joy_mask & 0x0FFF
                hori_mask = (joy_mask >> 12) & 0x0FFF

                # list which pads are “pressed”
                vert_pressed = [i for i in range(12) if (vert_mask >> i) & 1]
                hori_pressed = [i for i in range(12) if (hori_mask >> i) & 1]

                # print
                print(f"LED mask:    0x{led_mask:04X}")
                print(f"LED bitmask: 0x{led_bits}")
                print(f"Key mask:    0x{key_mask:04X}")
                print(f"Joy mask:    0x{joy_mask:06X}")
                print(f" Vertical:   bits {vert_pressed}")
                print(f" Horizontal: bits {hori_pressed}")
                print("-" * 40)

            time.sleep(0.05)

    except KeyboardInterrupt:
        pass
    finally:
        dev.close()

if __name__ == "__main__":
    main()
