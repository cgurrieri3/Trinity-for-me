import tkinter as tk
import os
import pickle
import subprocess

class OutletWidget:
    def __init__(self, root, LCF):
        self.popup = tk.Toplevel(root)
        self.popup.title("Outlet Control Page")

        self.LCF = LCF
        
        # Page title
        label = tk.Label(self.popup, text="Outlet Control Page")

        # Init buttons list
        self.buttons = []

        # Create the Wall cabinet frame with buttons
        rec1 = tk.Frame(self.popup, width=300, height=1000, bg="gray")
        rec1.grid(row=1,column=0,padx=10,pady=10)
        label1 = tk.Label(rec1, text="Wall Cabinet")
        label1.grid(row=1,column=0,padx=10,pady=10)

        
        # Create the rolling cabinet frame with buttons
        rec2 = tk.Frame(self.popup, width=300, height=1000, bg="gray")
        rec2.grid(row=1,column=1,padx=10,pady=10)
        label2 = tk.Label(rec2, text="Rolling Cabinet")
        label2.grid(row=1,column=1,padx=10,pady=10)

        # Define the outlet names
        OUTLET_NAMES = {
            rec1 : ['Netgear Switch','Control PC','Heating Mat','Open','WX Station','Open','Open','uSwitch Relays'],
            rec2 : ['Chiller','CT CPU','Open','Open','Open','Magna PS','MicroTCA','Netgear Switch']
        }

        # Check the status of the pdus from pickled file
        with open('outlet_status.pkl', 'rb') as f:
            status = pickle.load(f)
        print(status)
        
        # Create all the buttons
        for rec in [rec1, rec2]:
            for i in range(8):
                if rec == rec1:
                    try: # normal
                        color = "gray90" if status['wc'][f'{i+1}'] == "On" else "red" #check status
                    except: # if something is wrong with status
                        color = "gray90"
                    globals()[f'{rec}_button_{i}'] = tk.Button(rec, text=OUTLET_NAMES[rec1][i], bg=color, command=lambda lbl=i+1: self.button_click(lbl))
                    globals()[f'{rec}_button_{i}'].grid(row=i+2,column=0,padx=10,pady=5)
                else:
                    try:
                        color = "gray90" if status['rc'][f'{i+1}'] == "On" else "red" #check status
                    except:
                        color = "gray90"
                    globals()[f'{rec}_button_{i}'] = tk.Button(rec, text=OUTLET_NAMES[rec2][i], bg=color, command=lambda lbl=i+9: self.button_click(lbl))
                    globals()[f'{rec}_button_{i}'].grid(row=i+2,column=1,padx=10,pady=5)
                self.buttons.append(globals()[f'{rec}_button_{i}'])

        # Check the load of the pdus from pickled file
        with open('outlet_loads.pkl', 'rb') as f:
            status = pickle.load(f)
        print(status)
        # WC info
        self.wc_label_load = tk.Label(self.popup, text = f"Device Load: {status['wc']['Device Load']} A")
        self.wc_label_load.grid(row=11, column=0,sticky="w")
        self.wc_label_pf = tk.Label(self.popup, text = f"Power Factor: {status['wc']['Power Factor']}")
        self.wc_label_pf.grid(row=12, column=0,sticky="w")
        self.wc_label_energy = tk.Label(self.popup, text = f"Energy: {status['wc']['Energy ']} kWh")
        self.wc_label_energy.grid(row=14, column=0,sticky="w")

        # RC info
        self.rc_label_load = tk.Label(self.popup, text = f"Device Load: {status['rc']['Device Load']} A")
        self.rc_label_load.grid(row=11, column=1,sticky="w")
        self.rc_label_pf = tk.Label(self.popup, text = f"Power Factor: {status['rc']['Power Factor']}")
        self.rc_label_pf.grid(row=12, column=1,sticky="w")
        self.rc_label_energy = tk.Label(self.popup, text = f"Energy: {status['rc']['Energy ']} kWh")
        self.rc_label_energy.grid(row=14, column=1,sticky="w")

        # Variable to keep track of if we are in reboot outlet mode
        self.reboot = False
                
        # Create a check box that controls the whether to reboot the outlets or
        # turn them off individually
        self.var = tk.IntVar()
        checkbox = tk.Checkbutton(self.popup, text="Reboot Mode", variable=self.var, command=self.toggle_reboot)
        checkbox.grid()

        close_button = tk.Button(self.popup, text="Close", command=self.popup.destroy)
        close_button.grid()

        # Start the update_page function that will update the widget page every 60 seconds 
        self.update_page()

    def toggle_reboot(self):
        if self.var.get()==1:
            self.reboot = True
        else:
            self.reboot = False
        print(self.reboot)
            

    def button_click(self, label):
        print(self.reboot)
        # Check the load of the pdus from pickled file
        with open('outlet_status.pkl', 'rb') as f:
            status = pickle.load(f)
        if self.reboot:
            if self.buttons[label-1]["bg"] == "red":
                self.buttons[label-1]["bg"] = "gray90" # Reboot on off oulet turns it on
                if label < 9:
                    status['wc'][f'{label}'] = "On"
                    self.outlet_control('wc',label,'reboot')
                else:
                    status['rc'][f'{label-8}'] = "On"
                    self.outlet_control('rc',label-8,'reboot')
            else:
                self.buttons[label-1]["bg"] = "red" # Toggle button color to red
                if label < 9:
                    status['wc'][f'{label}'] = "On"
                    self.outlet_control('wc',label,'reboot')
                else:
                    status['rc'][f'{label}'] = "On"
                    self.outlet_control('rc',label-8,'reboot')
                self.buttons[label-1]['bg'] = "gray90"
        else:
            if self.buttons[label-1]["bg"] == "red":
                if label < 9:
                    status['wc'][f'{label}'] = "On"
                    self.outlet_control('wc',label,'on')
                else:
                    status['rc'][f'{label}'] = "On"
                    self.outlet_control('rc',label-8,'on')
                self.buttons[label-1]["bg"] = "gray90" # Reset button color to default
            else:
                if label < 9:
                    status['wc'][f'{label}'] = "Off"
                    self.outlet_control('wc',label,'off')
                else:
                    status['wc'][f'{label}'] = "Off"
                    self.outlet_control('rc',label-8,'off')
                self.buttons[label-1]["bg"] = "red" # Toggle button color to red

        # Write the edit outlet status pickle
        with open('outlet_status.pkl', 'wb') as f:
            pickle.dump(status,f)

    def update_page(self):
        # Check the load of the pdus from pickled file
        with open('outlet_status.pkl', 'rb') as f:
            status = pickle.load(f)
        print(status)

        # Update button colors
        for i,button in enumerate(self.buttons):
            if i < 8 : # wc
                try:
                    if status['wc'][f'{i+1}'] == "On":
                        button.config(bg="gray90")
                    else:
                        button.config(bg="red")
                except KeyError:
                    print("Warning - KeyError when updating. Wait for next update.")
            else: # rc
                try:
                    if status['rc'][f'{i-7}'] == "On":
                        button.config(bg="gray90")
                    else:
                        button.config(bg="red")
                except KeyError:
                    print("Warning - KeyError when updating. Wait for next update.")

        # Check the load of the pdus from pickled file
        with open('outlet_loads.pkl', 'rb') as f:
            load = pickle.load(f)
        print(load)

        # Update the labels
        self.wc_label_load.config(text = f"Device Load: {load['wc']['Device Load']} A")
        self.wc_label_pf.config(text = f"Power Factor: {load['wc']['Power Factor']}")
        self.wc_label_energy.config(text =  f"Energy: {load['wc']['Energy ']} kWh")
        self.rc_label_load.config(text = f"Device Load: {load['rc']['Device Load']} A")
        self.rc_label_pf.config(text = f"Power Factor: {load['rc']['Power Factor']}")
        self.rc_label_energy.config(text =  f"Energy: {load['rc']['Energy ']} kWh")

        # Schedule the next update after 60 seconds
        self.popup.after(60000,self.update_page)

    def outlet_control(self,cab,index,action):
        if self.LCF:
            full_cmd = ["/home/mpotts32/control_comp/outletcontrol.exp", f"{cab}", f"{index}", f"{action}"]
        else:
            sshpass_cmd = ["sshpass", "-p", os.environ['PHYS_PASS']]
            ssh_cmd = [
            "ssh", "-p", os.environ['PORT'],
            f"{os.environ['PHYS_USR']}@127.0.0.1",
            f"'/home/mpotts32/control_comp/outletcontrol.exp' {cab} {index} {action}"
        ]
            full_cmd = sshpass_cmd + ssh_cmd
        print(" ".join(full_cmd))
        proc = subprocess.Popen(full_cmd)
        
