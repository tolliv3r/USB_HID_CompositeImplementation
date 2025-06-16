import hid
import time
import sys

# adjust to your VID/PID and interface
VID, PID = 0x03EB, 0x2133
IFACE   = 2

def find_and_open():
    for d in hid.enumerate(VID, PID):
        if d['interface_number'] == IFACE:
            dev = hid.device()
            dev.open_path(d['path'])
            dev.set_nonblocking(False)
            return dev
    return None

def main():
    dev = find_and_open()
    if not dev:
        print(f"No device with VID=0x{VID:04X}, PID=0x{PID:04X}, IF={IFACE}")
        sys.exit(1)
    print("Opened device, listening for IN reports… (Ctrl-C to quit)")
    try:
        while True:
            rpt = dev.read(2)      # read ReportID+mask, or [mask] if no ReportID
            if rpt:
                # if your report is just one byte, rpt == [mask]
                mask = rpt[-1]
                bits = [(mask>>i)&1 for i in range(8)]
                print(f"raw={rpt!r}  mask=0x{mask:02X}  bits={bits}")
            time.sleep(0.05)
    except KeyboardInterrupt:
        pass
    finally:
        dev.close()

if __name__ == '__main__':
    main()
