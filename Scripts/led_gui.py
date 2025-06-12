import hid
import sys
import tkinter as tk
from functools import partial

VID, PID = 0x03EB, 0x2133
LED_IFACE_INDEX = 2

def find_led_path():
	devs = list(hid.enumerate(VID, PID))
	if len(devs) <= LED_IFACE_INDEX:
		raise RuntimeError(f"couldnt find nothin at index {LED_IFACE_INDEX}")
	return devs[LED_IFACE_INDEX]['path']


def send_mask(path, mask):
	device = hid.device()
	device.open_path(path)
	report = bytes([0x00, mask])
	device.write(report)
	device.close()


class LED_Toggler(tk.Tk):
	def __init__(self):
		super().__init__()
		self.title("Front-Panel LEDs")
		self.path = find_led_path()
		self.states = [False]*8

		for i in range(8):
			btn = tk.Button(self,
							text=f"LED {i+1}",
							width=8,
							relief="raised",
							command=partial(self.toggle, i))
			btn.grid(row=i//4, column=i%4, padx=5, pady=5)
			setattr(self, f"btn{i}", btn)

	def toggle(self, idx):
		self.states[idx] = not self.states[idx]
		btn = getattr(self, f"btn{idx}")
		btn.config(relief="sunken" if self.states[idx] else "raised")

		mask = 0
		for bit, on in enumerate(self.states):
			if on:
				mask |= (1 << bit)
		try:
			send_mask(self.path, mask)
		except Exception as e:
			tk.messagebox.showerror("HID Error", str(e))

if __name__ == "__main__":
	app = LED_Toggler()
	app.mainloop()