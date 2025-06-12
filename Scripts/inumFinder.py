import hid, sys

VID, PID = 0x03EB, 0x2133
mask = int(sys.argv[1], 0) if len(sys.argv) > 1 else 0xFF

devs = list(hid.enumerate(VID, PID))
print("Enumerated HID devices for {:04X}:{:04X}".format(VID, PID))
for idx, d in enumerate(devs):
    iface = d.get('interface_number', 'N/A')
    print(f"  [{idx}] path={d['path']}  iface={iface}")

if not devs:
    raise RuntimeError("no HID devices found :3")