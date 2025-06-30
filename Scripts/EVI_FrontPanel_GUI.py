#  ********************************************************************
# EVI Front-Panel GUI Script
#
# Author: Jackson Clary
# Purpose: GUI for controlling and monitoring the EVi Classic front-
#          -panel LEDs, keypad, and joystick via HID.
#
#----------------------------------------
# History:
#   Created June 12, 2025
#----------------------------------------
#  ********************************************************************

# ----------------------------------------
# --------------- Imports ----------------
# ----------------------------------------
import hid
import tkinter as tk
from tkinter import messagebox
from functools import partial

# ----------------------------------------
# -------------- Constants ---------------
# ---------------------------------------
VID = 0x03EB # USB Vendor ID for Atmel Devices
PID = 0x2133 # USB Product ID for XMEGA256A3U front panel

JSK_IFACE_NUMBER = 1    # USB HID interface number for joystick sub-device
LED_IFACE_NUMBER = 2    # USB HID interface number for LED sub-device

POLL_INTERVAL = 1       # ms

DOT_SIZE   = 8.5 # diameter of joystick indicator dots
DOT_MARGIN = 1   # margin inside joystick dot canvases

KEY_NAMES = [ # HID key names for keypad buttons
    "F1", "F2", "F3", "F4",
    "DISPLAY", "CANCEL", "ENTER", "CLEAR", "NULL"
]

# ----------------------------------------
# ----------- GUI Application -----------
# ----------------------------------------
class LED_Toggler(tk.Tk):
    # Tkinter GUI for front-panel LED control and input visualization.
    def __init__(self):
        super().__init__()

        # configure main window styff
        self.configure(bg="#4c9a8f")
        self.title("Front-Panel LEDs")

        # -------------------------------
        # HID LED interface
        # -------------------------------
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

        # ---------------------------------
        # HID Joystick interface
        # ---------------------------------
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

        # ---------------------------------
        # initialize LED state tracking
        # ---------------------------------
        self.states = [False] * 8 # track on/off state of each LED
        self.buttons = []         # store LED toggle button widgets
        self.skip_count = 0       # counter of skipped polls

        # ---------------------------------
        # LED toggle buttons
        # ---------------------------------
        for i in range(8):
            btn = tk.Button(
                self,
                text=f"LED {i+1}",
                width=8,
                relief=tk.RAISED,
                command=partial(self.toggle, i),
                bg="#e6e6e6"
            )
            btn.grid(row=1, column=i + 2, padx=5, pady=5)
            self.buttons.append(btn)

        # ---------------------------------
        # auxiliary buttons
        # ---------------------------------
        self.all_on_btn = tk.Button(
            self,
            text="All On",
            width=5,
            relief=tk.FLAT,
            bg="lightgreen",
            activebackground="lightgreen",
            command=self.set_all_on # turn on all LEDs
        )
        self.all_off_btn = tk.Button(
            self,
            text="All Off",
            width=5,
            relief=tk.FLAT,
            bg="lightcoral",
            activebackground="lightcoral",
            command=self.set_all_off # turn off all LEDs
        )
        self.start_btn = tk.Button(
            self,
            text="Start",
            width=5,
            relief=tk.FLAT,
            bg="#66b0ff",
            activebackground="#66b0ff",
            # fg="white",
            command=self.start_sequence # start idle sequence
        )
        self.stop_btn = tk.Button(
            self,
            text="Stop",
            width=5,
            relief=tk.FLAT,
            bg="#feae6d",
            activebackground="#feae6d",
            # fg="white",
            command=self.stop_activity # stop idle sequence
        )

        self.reset_btn = tk.Button(
            self,
            text="Reset",
            width=5,
            relief=tk.FLAT,
            bg="white",
            activebackground="white",
            command=self.reset_memory # clear key/joystick press history
        )

        # layout auxiliary buttons
        self.all_on_btn.grid(row=2, column=1, padx=3, pady=3)
        self.all_off_btn.grid(row=3, column=1, padx=3, pady=3)
        self.start_btn.grid_remove() # hide start button initially
        self.reset_btn.grid(row=5, column=1, padx=3, pady=3)

        # ---------------------------------
        # LED status indicators
        # ---------------------------------
        self.led_canvases = [] # canvases for LED state indicators
        self.led_ovals = []
        for i in range(8):
            canvas = tk.Canvas(
                self,
                width=20,
                height=20,
                highlightthickness=0,
                bg=self["bg"]
            )
            canvas.grid(row=0, column=i+2, pady=(0,5))
            oval = canvas.create_oval(2, 2, 18, 18, fill="gray") # default off
            self.led_canvases.append(canvas)
            self.led_ovals.append(oval)

        # status LED indicator
        self.status_canvas = tk.Canvas(
            self,
            width=20,
            height=20,
            highlightthickness=0,
            bg=self["bg"]
        )
        self.status_canvas.grid(row=0, column=1, pady=(0,5))
        self.status_oval = self.status_canvas.create_oval(2, 2, 18, 18,
                                                          fill="gray") # off = grey

        # ---------------------------------
        # status LED toggle button
        # ---------------------------------
        self.status_state = False
        self.status_btn = tk.Button(
            self,
            text="Status",
            width=8,
            relief=tk.RAISED,
            command=self.toggle_status,
            bg="#e6e6e6"
        )
        self.status_btn.grid(row=1, column=1, padx=5, pady=5)

        # ---------------------------------
        # keypad indicators
        # ---------------------------------
        self.key_states = [False] * len(KEY_NAMES)  # current press state
        self.key_labels = []                        # button widgets for keys
        self.key_pressed = [False] * len(KEY_NAMES) # press memory

        for i, name in enumerate(KEY_NAMES):
            btn = tk.Button(
                self,
                text=name,
                width=8,
                relief=tk.RAISED,
                bd=2,
                command=lambda: None, # do nothing on press
                bg="#64b4c4"
            )
            btn.config(bg="#64b4c4")
            key layout
            if i < 4:
                row = 5
                column = i + 2
            else:
                # row = i - 4
                row = 9 - i
                column = 10
            btn.grid(row=row, column=column, padx=5, pady=5)
            self.key_labels.append(btn)

        # ---------------------------------
        # joystick visualizer
        # ---------------------------------
        self.joy_size = 150
        joy_holder = tk.Frame(self, width=self.joy_size, height=self.joy_size, bg=self["bg"])
        joy_holder.grid_propagate(False)   
        joy_holder.grid(row=2, column=7, rowspan=4, columnspan=2, padx=0, pady=0)

        # canvas inside frame
        self.joy_canvas = tk.Canvas(
            joy_holder,
            width=self.joy_size,
            height=self.joy_size,
            bd=1,
            relief=tk.SUNKEN,
            bg="#e6e6e6"
        )
        self.joy_canvas.pack(fill="both", expand=True)

        # center dot
        r = 7
        cx = cy = self.joy_size // 2
        self.joy_dot = self.joy_canvas.create_oval(cx-r, cy-r, cx+r, cy+r, fill="teal")

        # vertical slider container
        self.v_slider_frame = tk.Frame(self,
                                       width=20,
                                       height=12*17,
                                       bg=self["bg"]
                                       # bg="white"
        )
        self.v_slider_frame.place(x=613,y=60)

        # horizontal slider container
        self.h_slider_frame = tk.Frame(self,
                                       width=12*17,
                                       height=20,
                                       bg=self["bg"]
                                       # bg="white"
        )
        self.h_slider_frame.place(x=455,y=218)

        # joystick button visualizers
        self.joyV_canvases = []
        self.joyV_ovals    = []
        self.joyH_canvases = []
        self.joyH_ovals    = []
        # vertical slider
        for i in range(12):
            cvs = tk.Canvas(self.v_slider_frame,
                            width=DOT_SIZE,
                            height=DOT_SIZE,
                            highlightthickness=0,
                            bg=self["bg"]
            )
            cvs.pack(pady=2)
            oval = cvs.create_oval(DOT_MARGIN,
                                   DOT_MARGIN,
                                   DOT_SIZE - DOT_MARGIN,
                                   DOT_SIZE - DOT_MARGIN,
                                   fill="gray",
                                   outline=self["bg"]
            )
            self.joyV_canvases.append(cvs)
            self.joyV_ovals.append(oval)
        # horizontal slider
        for i in range(12):
            cvs = tk.Canvas(self.h_slider_frame,
                            width=DOT_SIZE,
                            height=DOT_SIZE,
                            highlightthickness=0,
                            bg=self["bg"]
            )
            cvs.pack(side="left", padx=2)
            oval = cvs.create_oval(DOT_MARGIN,
                                   DOT_MARGIN,
                                   DOT_SIZE - DOT_MARGIN,
                                   DOT_SIZE - DOT_MARGIN,
                                   fill="gray",
                                   outline=self["bg"]
            )
            self.joyH_canvases.append(cvs)
            self.joyH_ovals.append(oval)

        # initialize press history for sliders
        self.joyV_pressed = [False]*12
        self.joyH_pressed = [False]*12

        # ---------------------------------
        # begin polling HID devices
        # ---------------------------------
        self.poll()

        # minimum window size
        self.update_idletasks()
        w = self.winfo_width()
        h = self.winfo_height()
        self.minsize(w, h + 14)

    def show_start(self): # show start button, hide stop button
        self.stop_btn.grid_remove()
        self.start_btn.grid(row=4, column=1, padx=3, pady=3)

    def show_stop(self): # show stop button, hide start button
        self.start_btn.grid_remove()
        self.stop_btn.grid(row=4, column=1, padx=3, pady=3)

    def toggle(self, idx): # toggle LED at index idx and send updated LED mask to device
        self.states[idx] = not self.states[idx] # flip local state

        mask = sum(1 << i for i, st in enumerate(self.states) if st) # build bitmask
        try:
            self.device.write(bytes([0x00, mask])) # send HID output report
        except Exception as e:
            messagebox.showerror("HID Write Error", str(e))
            return

        # skip the next bunch of polls (idk man buttons look weird otherwise)
        # self.skip_count = 100
        self.update_buttons()
        

    def toggle_status(self): # toggles status LED, sends updated bitmask to device
        self.status_state = not self.status_state
        code = 0x48 if self.status_state else 0x51
        try:
            self.device.write(bytes([0x00, 0x00, code]))
        except Exception as e:
            messagebox.showerror("HID cant write", str(e))
            return
        self.update_status_button()

    def set_all_on(self): # turns all LEDs on, sends updated map to device
        self.stop_activity()
        try:
            self.device.write(bytes([0x00, 0xFF, 0x00]))
            self.states = [True]*8
            self.update_buttons()
        except Exception as e:
            messagebox.showerror("HID Write Error", str(e))
    def set_all_off(self): # turns all LEDs off, sends updated map to device
        try:
            self.device.write(bytes([0x00, 0x00, 0x00]))
            self.states = [False]*8
            self.update_buttons()
        except Exception as e:
            messagebox.showerror("HID Write Error", str(e))

    def stop_activity(self): # stops idle sequence
        try:
            self.device.write(bytes([0x00, 0x00, 0x42]))
        except Exception as e:
            messagebox.showerror("HID oops", str(e))
    def start_sequence(self): # starts idle sequence
        try:
            self.device.write(bytes([0x00, 0x00, 0x45]))
        except Exception as e:
            messagebox.showerror("oopsies the HID", str(e))

    def status_on(self): # sets status LED on
        try: self.device.write(bytes([0x00, 0x00, 0x48]))
        except Exception as e:
            messagebox.showerror("HID NOOOOO", str(e))
    def status_off(self): # sets status LED off
        try: self.device.write(bytes([0x00, 0x00, 0x51]))
        except Exception as e:
            messagebox.showerror("nar hid", str(e))

    def update_buttons(self): # ensures each LED's button’s relief accurately reflects it's LED's state
        for i in range(8):
            btn = self.buttons[i]
            btn.config(relief=tk.SUNKEN if self.states[i] else tk.RAISED)
            color = "red" if self.states[i] else "gray"
            self.led_canvases[i].itemconfig(self.led_ovals[i], fill=color)
    def update_status_button(self): # ensure status LED's button accurately reflects status LED's state
        self.status_btn.config(
            relief=tk.SUNKEN if self.status_state else tk.RAISED
        )
        color = "yellow" if self.status_state else "gray"
        self.status_canvas.itemconfig(self.status_oval, fill=color)

    def reset_memory(self): # resets key/joystick indicator memory
        self.joyV_pressed = [False]*12
        self.joyH_pressed = [False]*12
        self.key_pressed  = [False]*len(KEY_NAMES)

    def poll(self): # reads the most recent IN report from device
        rpt = self.device.read(7)
        if rpt and len(rpt) >= 7:
            if self.skip_count > 0:
                # skip however many polls as specified
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
                    if pressed:
                        self.key_pressed[i] = True
                    self.key_states[i] = pressed
                    key_bg = "#64b4c4" if pressed else ("gray" if self.key_pressed[i] else "#64b4c4")
                    lbl.config(
                        relief=tk.SUNKEN if pressed else tk.RAISED,
                        bg=key_bg
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
                    if pressed:
                        self.joyV_pressed[i] = True
                        color = "white"
                    else:
                        color = "#ababab" if self.joyV_pressed[i] else "#192166"
                    self.joyV_canvases[i].itemconfig(oval, fill=color)
                #update horizontal indicators
                for i, oval in enumerate(self.joyH_ovals):
                    pressed = bool((jstk_bits >> (i + 12)) & 1)
                    if pressed:
                        self.joyH_pressed[i] = True
                        color = "white"
                    else:
                        color = "#ababab" if self.joyH_pressed[i] else "#192166"
                    self.joyH_canvases[i].itemconfig(oval, fill=color)

        # schedule next poll
        self.after(POLL_INTERVAL, self.poll)

    def on_closing(self): # shuts down GUI
        if self.device:
            self.device.close()
        self.destroy()

if __name__ == "__main__":
    app = LED_Toggler()
    app.protocol("WM_DELETE_WINDOW", app.on_closing)
    app.mainloop()
