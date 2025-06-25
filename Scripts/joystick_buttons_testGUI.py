import hid
import tkinter as tk
from tkinter import messagebox
from functools import partial

VID = 0x03EB
PID = 0x2133

JSK_IFACE_NUMBER = 1    # USB device iNumber for the joystick
LED_IFACE_NUMBER = 2    # USB device iNumber for the LEDs

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

        self.joystick = None
        for d in hid.enumerate(VID, PID):
            if d.get('interface_number') == JSK_IFACE_NUMBER:
                self.joystick = hid.device()
                self.joystick.open_path(d['path'])
                self.joystick.set_nonblocking(True)
                break
        if not self.joystick:
            messagebox.showwarning("Joystick not found",
                f"Joystick interface {JSK_IFACE_NUMBER} missing.")

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
            btn.grid(row=1, column=i + 2, padx=5, pady=5)
            self.buttons.append(btn)

        # auxiliary buttons
        self.all_on_btn = tk.Button(
            self,
            text="All On",
            width=5,
            relief=tk.FLAT,
            bg="lightgreen",
            command=self.set_all_on
        )
        self.all_off_btn = tk.Button(
            self,
            text="All Off",
            width=5,
            relief=tk.FLAT,
            bg="lightcoral",
            command=self.set_all_off
        )
        self.start_btn = tk.Button(
            self,
            text="Start",
            width=5,
            relief=tk.FLAT,
            bg="#66b0ff",
            # fg="white",
            command=self.start_sequence
        )
        self.stop_btn = tk.Button(
            self,
            text="Stop",
            width=5,
            relief=tk.FLAT,
            bg="#feae6d",
            # fg="white",
            command=self.stop_activity
        )

        self.all_on_btn.grid(row=2, column=1, padx=3, pady=3)
        self.all_off_btn.grid(row=3, column=1, padx=3, pady=3)

        self.start_btn.grid_remove()

        # LED indicators
        self.led_canvases = []
        self.led_ovals = []
        for i in range(8):
            canvas = tk.Canvas(
                self,
                width=20,
                height=20,
                highlightthickness=0
            )
            canvas.grid(row=0, column=i+2, pady=(0,5))
            oval = canvas.create_oval(2, 2, 18, 18, fill="gray")
            self.led_canvases.append(canvas)
            self.led_ovals.append(oval)

        # status LED indicator
        self.status_canvas = tk.Canvas(
            self,
            width=20,
            height=20,
            highlightthickness=0
        )
        self.status_canvas.grid(row=0, column=1, pady=(0,5))
        self.status_oval = self.status_canvas.create_oval(2, 2, 18, 18, fill="gray") # off = grey

        # status LED toggle button
        self.status_state = False
        self.status_btn = tk.Button(
            self,
            text="Status",
            width=8,
            relief=tk.RAISED,
            command=self.toggle_status
        )
        self.status_btn.grid(row=1, column=1, padx=5, pady=5)

        # key indicators
        self.key_states = [False] * len(KEY_NAMES)
        self.key_labels = []
        for i, name in enumerate(KEY_NAMES):
            btn = tk.Button(
                self,
                text=name,
                width=8,
                relief=tk.RAISED,
                bd=2,
                command=lambda: None
            )
            btn.config(bg="lightblue")
            if i < 4:
                row = 5
                column = i + 2
            else:
                # row = i - 4
                row = 9 - i
                column = 10
            btn.grid(row=row, column=column, padx=5, pady=5)
            self.key_labels.append(btn)

        # joystick visualizer
        self.joy_size = 150

        # container frame
        joy_holder = tk.Frame(self, width=self.joy_size, height=self.joy_size)
        joy_holder.grid_propagate(False)   
        joy_holder.grid(row=2, column=7, rowspan=5, columnspan=2, pady=(10,0))

        # canvas inside frame
        self.joy_canvas = tk.Canvas(
            joy_holder,
            width=self.joy_size,
            height=self.joy_size,
            bd=1,
            relief=tk.SUNKEN
        )
        self.joy_canvas.pack(fill="both", expand=True)
        self.joy_canvas.create_rectangle(0, 0, self.joy_size, self.joy_size)
        r = 6
        cx = cy = self.joy_size // 2
        self.joy_dot = self.joy_canvas.create_oval(cx-r, cy-r, cx+r, cy+r, fill="blue")

        # joystick button visualizers
        self.joyV_canvases = []
        self.joyV_ovals    = []
        self.joyH_canvases = []
        self.joyH_ovals    = []
        # vertical slider
        for i in range(12):
            cvs = tk.Canvas(self, width=15, height=15, highlightthickness=0)
            cvs.grid(row=7, column=i+11, padx=2, pady=2)
            oval = cvs.create_oval(2,2,13,13, fill="gray")
            self.joyV_canvases.append(cvs)
            self.joyV_ovals.append(oval)
        # horizontal slider
        for i in range(12):
            cvs = tk.Canvas(self, width=15, height=15, highlightthickness=0)
            cvs.grid(row=8, column=i+11, padx=2, pady=2)
            oval = cvs.create_oval(2,2,13,13, fill="gray")
            self.joyH_canvases.append(cvs)
            self.joyH_ovals   .append(oval)

        # begin polling
        self.poll()

    def show_start(self):
        self.stop_btn.grid_remove()
        self.start_btn.grid(row=4, column=1, padx=3, pady=3)

    def show_stop(self):
        self.start_btn.grid_remove()
        self.stop_btn.grid(row=4, column=1, padx=3, pady=3)

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

        # skip the next bunch of polls (idk man buttons look weird otherwise)
        self.skip_count = 100
        self.update_buttons()
        

    def toggle_status(self):
        self.status_state = not self.status_state

        code = 0x82 if self.status_state else 0x83

        try:
            self.device.write(bytes([0x00, code]))
        except Exception as e:
            messagebox.showerror("HID cant write", str(e))
            return
        
        self.skip_count = 100
        self.update_buttons()
        self.update_status_button()

    def set_all_on(self):
        self.stop_activity()
        try:
            self.device.write(bytes([0x00, 0xFF]))
            self.states = [True]*8
            self.update_buttons()
        except Exception as e:
            messagebox.showerror("HID Write Error", str(e))
    def set_all_off(self):
        try:
            self.device.write(bytes([0x00, 0x00]))
            self.states = [False]*8
            self.update_buttons()
        except Exception as e:
            messagebox.showerror("HID Write Error", str(e))

    def stop_activity(self):
        try:
            self.device.write(bytes([0x00, 0x80]))
        except Exception as e:
            messagebox.showerror("HID oops", str(e))
    def start_sequence(self):
        try:
            self.device.write(bytes([0x00, 0x81]))
        except Exception as e:
            messagebox.showerror("oopsies the HID", str(e))

    def status_on(self):
        try: self.device.write(bytes([0x00, 0x82]))
        except Exception as e:
            messagebox.showerror("HID NOOOOO", str(e))
    def status_off(self):
        try: self.device.write(bytes([0x00, 0x83]))
        except Exception as e:
            messagebox.showerror("nar hid", str(e))

    def update_buttons(self):
        # reflect self.states in each button’s relief
        for i in range(8):
            btn = self.buttons[i]
            btn.config(relief=tk.SUNKEN if self.states[i] else tk.RAISED)
            color = "red" if self.states[i] else "gray"
            self.led_canvases[i].itemconfig(self.led_ovals[i], fill=color)
    def update_status_button(self):
        self.status_btn.config(
            relief=tk.SUNKEN if self.status_state else tk.RAISED
        )
        color = "yellow" if self.status_state else "gray"
        self.status_canvas.itemconfig(self.status_oval, fill=color)

    def poll(self):
        rpt = self.device.read(7)
        if rpt and len(rpt) >= 7:
            if self.skip_count > 0:
                # drop stale values as many times as needed
                self.skip_count -= 1
            else:
                leds   = rpt[0]
                status = rpt[1]
                keysLo = rpt[2]
                keysHi = rpt[3]
                jstkLo = rpt[4]
                jstkMi = rpt[5]
                jstkHi = rpt[6]
                

                # update LED states & visuals
                for i in range(8):
                    self.states[i] = bool((leds >> i) & 1)
                self.update_buttons()

                # update status LED indicator
                self.status_state = bool(status & 0x01)       
                self.update_status_button()

                # update key indicators
                key_bits = keysLo | (keysHi << 8)
                for i, lbl in enumerate(self.key_labels):
                    pressed = bool((key_bits >> i) & 1)
                    self.key_states[i] = pressed
                    lbl.config(
                        relief=tk.SUNKEN if pressed else tk.RAISED
                    )

                if status & 0x02:
                    self.show_stop()
                else:
                    self.show_start()

                # read & update joystick
                if self.joystick:
                    js = self.joystick.read(2)
                    if js and len(js) >= 2:
                        x_raw, y_raw = js[0], js[1]
                        px = x_raw * self.joy_size / 255
                        py = y_raw * self.joy_size / 255
                        r = 6
                        self.joy_canvas.coords(
                            self.joy_dot,
                            px-r,
                            py-r,
                            px+r,
                            py+r
                        )

                # update vertical indicators
                jstk_bits = jstkLo | (jstkMi << 8) | (jstkHi << 16)
                for i, oval in enumerate(self.joyV_ovals):
                    pressed = bool((jstk_bits >> i) & 1)
                    color   = "white" if pressed else "darkblue"
                    self.joyV_canvases[i].itemconfig(oval, fill=color)
                #update horizontal indicators
                for i, oval in enumerate(self.joyH_ovals):
                    pressed = bool((jstk_bits >> (i + 12)) & 1)
                    color   = "white" if pressed else "darkblue"
                    self.joyH_canvases[i].itemconfig(oval, fill=color)

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
