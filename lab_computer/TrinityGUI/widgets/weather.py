import tkinter as tk
import os
from PIL import ImageTk, Image
from .more_weather import MoreWeatherWidget

class WeatherWidget:
    def __init__(self, root):
        self.root = root
        self.popup = tk.Toplevel(root)
        self.popup.title("Weather")

        global comp_img
        comp_img = ImageTk.PhotoImage(Image.open(f"{os.environ['WPDIR']}/wind_direction.png").resize((350, 350))) # get the image import into tkinter
        self.comp_label = tk.Label(self.popup, image = comp_img)
        self.comp_label.grid(row = 0, column = 0)

        global over_img
        over_img = ImageTk.PhotoImage(Image.open(f"{os.environ['WPDIR']}/wind_label_readout.png").resize((350, 350))) # get the image import into tkinter
        self.over_label = tk.Label(self.popup, image = over_img)
        self.over_label.grid(row = 0, column = 1)

        plots_button = tk.Button(self.popup, text="WX Plots", command=self.open_more_wx)
        plots_button.grid(row = 1, column = 0)
        
        close_button = tk.Button(self.popup, text="Close", command=self.popup.destroy)
        close_button.grid(row = 1, column = 1)

        # Start the update_img function that will update the widget page every 60 seconds 
        self.update_img()

    def open_more_wx(self):
        MoreWeatherWidget(self.root)

    def update_img(self):
        comp_img = ImageTk.PhotoImage(Image.open(f"{os.environ['WPDIR']}/wind_direction.png").resize((350, 350))) # get the image import into tkinter
        self.comp_label.config(image = comp_img) # Create a Label Widget to display the text or Image
        self.comp_label.image = comp_img # Keep a reference to avoid garbage collection

        over_img = ImageTk.PhotoImage(Image.open(f"{os.environ['WPDIR']}/wind_label_readout.png").resize((350, 350))) # get the image import into tkinter
        self.over_label.config(image = over_img) # Create a Label Widget to display the text or Image
        self.over_label.image = over_img # Keep a reference to avoid garbage collection
        
        # Schedule the next update after 60 seconds
        self.popup.after(60000,self.update_img)
