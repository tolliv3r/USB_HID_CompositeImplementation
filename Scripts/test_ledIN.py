#!/usr/bin/env python3
import hid
import sys
import time

# 1) Replace these with your device’s VID & PID:
VID = 0x03EB
PID = 0x2133

def find_and_open(vid, pid):
    for d in hid.enumerate():
        if d['vendor_id'] == vid and d['product_id'] == pid:
            dev = hid.device(path=d['path'])
            print(f"Opened {hex(vid)}:{hex(pid)}")
            return dev
    print(f"Device {hex(vid)}:{hex(pid)} not found", file=sys.stderr)
    sys.exit(1)

def pretty_bits(lo, hi):
    """Return a string of 16 bits, hi:msb → lo:lsb."""
    return f"{hi:08b} {lo:08b}"

def main():
    dev = find_and_open(VID, PID)
    dev.set_nonblocking(True)

    print("Reading 2-byte LED map.  Press Ctrl-C to quit.")
    try:
        while True:
            data = dev.read(2)   # returns a list of up to 2 ints [LSB, MSB]
            if data:
                lo, hi = data[0], data[1]
                bits = pretty_bits(lo, hi)
                # bit 0–7 = LEDs, bit 8 = status LED
                status = (hi & 0x01)
                print(f"RAW: {bits}   status={(status and 'ON' or 'off')}")
            time.sleep(0.05)
    except KeyboardInterrupt:
        print("\nDone.")
    finally:
        dev.close()

if __name__ == "__main__":
    main()
