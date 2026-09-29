import tkinter as tk
import os
from PIL import ImageTk, Image

class CameraWidget:
    def __init__(self, root):
        self.popup = tk.Toplevel(root)
        self.popup.title("Cameras") 

        # Define the cam names
        # Cameras can easily be added here! Everything in this class is dynamic
        self.CAMS = ['IN','OUT']

        # Create a number of labels with images equal to the length of self.CAMS
        for i,cam in enumerate(self.CAMS):
            img     = self.path_to_img(cam)
            try:
                # dynamically keep a reference of all images
                globals()[cam] = ImageTk.PhotoImage(Image.open(img).resize((550, 350))) # get the image import into tkinter

                # Create a Label Widget to display the text or Image
                globals()[f'{cam}_label'] = tk.Label(self.popup, image = globals()[cam])

                globals()[f'{cam}_label'].pack()
            except OSError:
                print("Warning - OSError: Image is truncated.")

        # Close button        
        close_button = tk.Button(self.popup, text="Close", command=self.popup.destroy)
        close_button.pack(pady=10)

        # Start the update_img function that will update the widget page every 60 seconds 
        self.update_img()

    def path_to_img(self,cam):
        CAMDIR = os.environ['CAMDIR']
        img_path = f'{CAMDIR}/{cam}CAM.jpg'
        return img_path

    def update_img(self):
        for i,cam in enumerate(self.CAMS):
            img     = self.path_to_img(cam)
            try:
                # dynamically keep a reference of all images
                globals()[cam] = ImageTk.PhotoImage(Image.open(img).resize((550, 350))) # get the image import into tkinter

                # Create a Label Widget to display the text or Image
                globals()[f'{cam}_label'].config(image = globals()[cam])
                
                # Keep a reference to avoid garbage collection
                globals()[f'{cam}_label'].image = globals()[cam] 
            except OSError:
                print("Warning - OSError: Image is truncated.")

        # Schedule the next update after 60 seconds
        self.popup.after(60000,self.update_img) 
        
