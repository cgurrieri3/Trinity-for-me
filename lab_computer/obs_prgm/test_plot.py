
import glob
import os
import pandas as pd
import matplotlib
matplotlib.get_backend()
matplotlib.use('TkAgg')

import matplotlib.pyplot as plt
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
import warnings
import shutup 
from collections import deque
shutup.please()


warnings.filterwarnings("ignore")





def get_most_recent_file(folder_path):
    # Use glob to list all files in the folder sorted by modification time
    files = glob.glob(os.path.join(folder_path, '*'))
    files.sort(key=os.path.getmtime, reverse=True)

    # Check if there are any files in the folder
    if files:
        most_recent_file = files[0]
        return most_recent_file
    else:
        print("No files found in the folder.")
        return None



folder_lab_machine = "/home/mpotts32/obs_prgm/rcLogs/"
most_recent_file = get_most_recent_file(f'{folder_lab_machine}Tscan/')
#print(most_recent_file)
df = pd.read_csv(f'{most_recent_file}', delimiter='\t\t',skiprows = 1)
df.columns = names=['Steps', 'Threshold', 'TriggerRate']
print(df)
plt.plot(df.Threshold, df.TriggerRate,'o-',color = 'g')
plt.title('Selecting Trigger Rate')
plt.xlabel('Threshold')
plt.ylabel('Trigger Rate')
plt.ylim([0,50])
plt.savefig(f'{folder_lab_machine}Plots/Tscan_now.png')
plt.show()