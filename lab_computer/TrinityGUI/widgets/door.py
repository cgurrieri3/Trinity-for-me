import tkinter as tk
import os
import subprocess

class DoorWidget:
    def __init__(self, root, LCF):
        self.popup = tk.Toplevel(root)
        self.popup.title("Door Control")

        self.LCF = LCF
        
        open_button = tk.Button(self.popup, text="Open Door", command=lambda dir='up': self.door_control(dir))
        open_button.grid(row = 0, column = 0)

        close_button = tk.Button(self.popup, text="Close Door", command=lambda dir='down': self.door_control(dir))
        close_button.grid(row = 0, column = 1)

        close_button = tk.Button(self.popup, text="Close", command=self.popup.destroy)
        close_button.grid(row = 0, column = 2)

    def door_control(self,direction):
        if self.LCF:
            full_cmd = ["/home/mpotts32/control_comp/doorcontrol.exp", f"{direction}"]
        else:    
            sshpass_cmd = ["sshpass", "-p", os.environ['PHYS_PASS']]
            ssh_cmd = [
                "ssh", "-p", os.environ['PORT'],
                f"{os.environ['PHYS_USR']}@127.0.0.1",
                f"'/home/mpotts32/control_comp/doorcontrol.exp' {direction}"
            ]
            full_cmd = sshpass_cmd + ssh_cmd
        print(" ".join(full_cmd))
        proc = subprocess.Popen(full_cmd)
