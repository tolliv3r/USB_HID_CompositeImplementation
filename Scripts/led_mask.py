import hid
import sys

VID = 0x03EB
PID = 0x2133

def main():
	if len(sys.argv) != 2:
		print(f"Usage: {sys.argv[0]} <mask>")
		sys.exit(1)

	try:
		mask = int(sys.argv[1], 0) & 0xFF
	except ValueError:
		print("gotta be an integer")
		sys.exit(1)

	report = [0x00, mask, 0x00]

	try:
		h = hid.device()
		h.open(VID, PID)
	except Exception as e:
		print(f"failed to open device (VID=0x{VID}, PID=0x{PID}): {e}")
		sys.exit(1)

	try:
		sent = h.write(report)
	except Exception as e:
		print(f"failed to send report: {e}")
	finally:
		h.close()

if __name__ == "__main__":
	main()