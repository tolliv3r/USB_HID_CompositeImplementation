import hid
import tkinter as tk
from tkinter import messagebox
from functools import partial

VID = 0x03EB
PID = 0x2133
LED_IFACE_NUMBER = 2     # USB device iNumber for the LEDs
POLL_INTERVAL = 1       # ms

KEY_NAMES = [
    "F1", "F2", "F3", "F4",
    "DISPLAY", "CANCEL", "ENTER", "CLEAR", "NULL"
]

class LED_Toggler(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("Front-Panel LEDs")

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

        # track each LED’s on/off
        self.states = [False] * 8
        self.buttons = []
        self.skip_count = 0

        # 8 buttons
        for i in range(8):
            btn = tk.Button(
                self,
                text=f"LED {i+1}",
                width=8,
                relief=tk.RAISED,
                command=partial(self.toggle, i)
            )
            btn.grid(row=i//4, column=i%4 + 2, padx=5, pady=5)
            self.buttons.append(btn)

        # status LED indicator
        self.status_canvas = tk.Canvas(
            self,
            width=20,
            height=32,
            highlightthickness=0
        )
        self.status_canvas.grid(row=0, column=1, pady=(10,0))
        self.status_oval = self.status_canvas.create_oval(2,2,18,18,fill="gray") # off = grey

        # key indicators
        self.key_states = [False] * len(KEY_NAMES)
        self.key_labels = []
        for i, name in enumerate(KEY_NAMES):
            lbl = tk.Label(
                self,
                text=name,
                width=8,
                bg="gray",
                relief=tk.SUNKEN
            )
            lbl.grid(row=(i//4) + 5, column=i%4, padx=5, pady=5)
            self.key_labels.append(lbl)

        # begin polling
        self.poll()

    def toggle(self, idx):
        # flip local state
        self.states[idx] = not self.states[idx]

        # build the mask & write it
        mask = sum(1 << i for i, st in enumerate(self.states) if st)
        try:
            self.device.write(bytes([0x00, mask]))
        except Exception as e:
            messagebox.showerror("HID Write Error", str(e))
            return

        self.update_buttons()
        # skip the next bunch of polls (idk man buttons look weird otherwise)
        self.skip_count = 100

    def update_buttons(self):
        # reflect self.states in each button’s relief
        for i, btn in enumerate(self.buttons):
            btn.config(relief=tk.SUNKEN if self.states[i] else tk.RAISED)

    def poll(self):
        rpt = self.device.read(4)
        if rpt and len(rpt) >= 4:
            if self.skip_count > 0:
                # drop stale values as many times as needed
                self.skip_count -= 1
            else:
                leds, status, keysLo, keysHi = rpt[0], rpt[1], rpt[2], rpt[3]

                # update LED states & visuals
                for i in range(8):
                    self.states[i] = bool((leds >> i) & 1)
                self.update_buttons()

                # update status LED indicator
                status_on = bool(status & 0x01)
                color = "yellow" if status_on else "gray"
                self.status_canvas.itemconfig(self.status_oval, fill=color)

                # update key indicators
                key_bits = keysLo | (keysHi << 8)
                for i, lbl in enumerate(self.key_labels):
                    pressed = bool((key_bits >> i) & 1)
                    self.key_states[i] = pressed
                    lbl.config(bg="blue" if pressed else "gray")

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
