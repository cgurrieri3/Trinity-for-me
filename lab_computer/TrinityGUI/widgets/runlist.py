import tkinter as tk

class RunlistWidget:
    def __init__(self,root):
        self.popup = tk.Toplevel(root)
        self.popup.title("Run List")

        #################################################################
        ###### List of things that need to happen to make run happen ####
        # Placing a Listbox that has all the steps
        self.listbox = tk.Listbox(self.popup,
                                  bg = "grey",
                                  height = 20, # number of lines
                                  width = 50,  # number of characters
                                  activestyle = 'dotbox',
                                  fg = "yellow",
                                  exportselection=False)
        self.listbox.grid(row=1,column=0)
        steps = ["Turn on CT CPU",
                 "Turn on chiller",
                 "Turn on Micro TCH Crate",
                 "Turn on Magna PS",
                 "Power on LVPS",
                 "Start Master Control",
                 "Start CTM",
                 "Sequence init",
                 "Sequence config",
                 "SIAB HV power ON",
                 "Check Weather",
                 "Open Pod bay door",
                 "Enable Flasher",
                 "Start night-sky data",
                 "Stop night-sky data",
                 "Disable Flasher",
                 "Close Pod pay door",
                 "Sequence kill",
                 "SIAB HV power OFF",
                 "Power off LVPS",
                 "Turn off Magna PS",
                 "Turn off Micro TCH Crate",
                 "Turn off chiller",
                 "Turn off CT CPU"]
        for step in steps:
            self.listbox.insert(tk.END,step)

        # Set the initial selection to the first item
        self.index = 0
        self.listbox.select_set(self.index)

        # Set paused state
        self.paused = False

        self.listbox.bind('<<ListboxSelect>>',self.on_select)

        # Create Start Button
        start_button = tk.Button(self.popup,text="Start",command=self.start_execution)
        start_button.grid(row=0,column=0)
        
        # Create Pause Button
        pause_button = tk.Button(self.popup,text="Pause",command=self.pause_execution)
        pause_button.grid(row=0,column=1)

        close_button = tk.Button(self.popup, text="Close", command=self.popup.destroy)
        close_button.grid(row = 0, column = 2)

    def on_select(self,event):
        # Get the selected item from the Listbox
        selected_item = self.listbox.get(self.listbox.curselection())
        
        # Set the index to what is selected
        self.index = int(self.listbox.curselection()[0])
        print("Index: ",self.index)
        print("Selected Item: ", selected_item)

    def start_execution(self):
        # Start the execution from the selected item
        self.paused = False
        if self.index < self.listbox.size() - 1:
            self.popup.after(1000, self.select_next_item)

    def select_next_item(self):
        # Select the current item in the Listbox
        self.listbox.select_clear(0,tk.END)
        self.listbox.selection_set(self.index)
        
        # Check if there are more items to process
        if self.index < self.listbox.size() - 1:
            #Check if execution is paused
            if not self.paused:
                # Get the selected item from the Listbox
                selected_item = self.listbox.get(self.listbox.curselection())
                
                # Perform the desired action for the selected item
                print(f"Executing step for {selected_item}...\n")
                
                # Move to the next item after a delay of 1 second
                # to be replaced later with commands to be executed
                print("stuff is happening...")

                # Move on to the next index in 1 second
                self.index += 1
                self.popup.after(1000, self.select_next_item)
            else:
                print(f"Execution paused.\n")
        else:
            print(f"Execution compledted.\n")

    def pause_execution(self):
        # Toggle the paused state
        self.paused = not self.paused
        if not self.paused:
            # Resume the eecution
            self.select_next_item()
