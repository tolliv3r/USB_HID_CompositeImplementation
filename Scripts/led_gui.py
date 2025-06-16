import hid
import tkinter as tk
from tkinter import messagebox
from functools import partial

VID = 0x03EB
PID = 0x2133
LED_IFACE_NUMBER = 2     # USB device iNumber for the LEDs
POLL_INTERVAL = 10       # ms

class LED_Toggler(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("Front-Panel LEDs")

        # track each LED’s on/off
        self.states = [False] * 8
        self.buttons = []
        self.skip_count = 0

        # open the HID LED interface
        self.device = None
        for d in hid.enumerate(VID, PID):
            if d.get('interface_number') == LED_IFACE_NUMBER:
                self.device = hid.device()
                self.device.open_path(d['path'])
                self.device.set_nonblocking(True)
                break

        if not self.device:
            messagebox.showerror("Error", f"LED interface {LED_IFACE_NUMBER} not found.")
            self.destroy()
            return

        # build 8 plain buttons
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

        # kick off polling
        self.poll()

    def toggle(self, idx):
        # flip our local state
        self.states[idx] = not self.states[idx]

        # build the mask & write it
        mask = sum(1 << i for i, st in enumerate(self.states) if st)
        try:
            self.device.write(bytes([0x00, mask]))
        except Exception as e:
            messagebox.showerror("HID Write Error", str(e))
            return

        # immediately update visuals (no bounce)
        self.update_buttons()
        # skip the next 40 polls (for visual buttons fluidity)
        self.skip_count = 40

    def update_buttons(self):
        # reflect self.states in each button’s relief
        for i, btn in enumerate(self.buttons):
            btn.config(relief=tk.SUNKEN if self.states[i] else tk.RAISED)

    def poll(self):
        # try reading exactly one data byte (should always be the case)
        rpt = self.device.read(1)
        if rpt:
            if self.skip_count > 0:
                # drop stale values as many times as needed
                self.skip_count -= 1
            else:
                new_mask = rpt[0]
                # update local states & visuals
                for i in range(8):
                    self.states[i] = bool((new_mask >> i) & 1)
                self.update_buttons()
        # schedule next poll
        self.after(POLL_INTERVAL, self.poll)

    def on_closing(self):
        if self.device:
            self.device.close()
        self.destroy()

if __name__ == "__main__":
    app = LED_Toggler()
    app.protocol("WM_DELETE_WINDOW", app.on_closing)
    app.mainloop()
