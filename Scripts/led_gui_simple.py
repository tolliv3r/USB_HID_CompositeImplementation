import hid
import sys
import tkinter as tk
from tkinter import messagebox
from functools import partial

VID = 0x03EB
PID = 0x2133
LED_IFACE_INDEX = 2
POLL_INTERVAL = 50

class LED_Toggler(tk.Tk):
	def __init__(self):
		super().__init__()
		self.title("Front-Panel LEDs")

		self.device = None
		try:
			for d in hid.enumerate(VID, PID):
				if d.get('interface_number') == LED_IFACE_INDEX:
					self.device = hid.device()
					self.device.open_path(d['path'])
					self.device.set_nonblocking(True)
					break
		except Exception as e:
			messagebox.showerror("HID oop couldn't open interface {e}")
			self.destroy()
			return

		if not self.device:
			messagebox.showerror("Erroneous cuz interface #{LED_Toggler} not found")
			self.destroy()
			return

		self.states = [False] * 8
		self.buttons = []

		for i in range(8):
			btn = tk.Button(
				self,
				text=f"LED {i+1}",
				width=8,
				relief=tk.RAISED,
				command=partial(self.toggle, i)
			)
			btn.grid(row=i//4, column=i%4, padx=5, pady=5)
			self.buttons.append(btn)

		self.poll()

	def poll(self):
		try:
			rpt = self.device.read(7)
		except Exception:
			rpt = None

		if rpt and len(rpt) >= 1:
			mask = rpt[0]
			for i in range(8):
				self.states[i] = bool((mask >> i) & 1)
			self.update_buttons()
		self.after(POLL_INTERVAL, self.poll)

	def update_buttons(self):
		for i, btn in enumerate(self.buttons):
			btn.config(relief=tk.SUNKEN if self.states[i] else tk.RAISED)

	def toggle(self, idx):
		self.states[idx] = not self.states[idx]
		self.update_buttons()

		mask = 0
		for bit, on in enumerate(self.states):
			if on:
				mask |= (1 << bit)
		try:
			self.device.write(bytes([0x00, mask]))
		except Exception as e:
			messagebox.showerror("HID error", str(e))

	def on_closing(self):
		if self.device:
			self.device.close()
		self.destroy()

if __name__ == "__main__":
	app = LED_Toggler()
	app.protocol("WM_DELETE_WINDOW", app.on_closing)
	app.mainloop()