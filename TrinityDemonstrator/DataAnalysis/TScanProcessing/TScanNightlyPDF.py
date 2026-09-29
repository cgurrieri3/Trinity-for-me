import os
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages
import pandas as pd
import argparse

def parse_arguments():
    # Create ArgumentParser object
    parser = argparse.ArgumentParser(description='Get data recorded for a single night')

    # Add arguments
    parser.add_argument('-d', '--date', help='Date you request(YYMMDD)', required=True)
    #parser.add_argument('-n', '--number', help='A number', type=int, required=True)
    #parser.add_argument('-s', '--string', help='A string', required=True)
    parser.add_argument("-ifile", "--input_files", help = "Misc Data .csv for SM")
    # parser.add_argument("-ofile","--output_files", help = "output filename")

    # Parse arguments from the command line
    args = parser.parse_args()
    return args



def list_folders_in_directory(directory):
    try:
        # Get a list of all entries in the directory
        entries = os.listdir(directory)
        # Filter out only directories
        folders = [entry for entry in entries if os.path.isdir(os.path.join(directory, entry))]
        return folders
    except Exception as e:
        print(f"Error: {e}")
        return []

def get_text_files(directory):
    try:
        # Get a list of all entries in the directory
        entries = os.listdir(directory)
        # Filter out only .txt files
        text_files = [entry for entry in entries if entry.endswith('.txt')]
        #print(text_files)
        return text_files[0]
    except Exception as e:
        print(f"Error: {e}")
        return []

def get_data(file):
	df = pd.read_csv(file, delimiter='\t\t',skiprows = 1,engine='python')
	df.columns = names=['Steps', 'Threshold', 'TriggerRate']
	#print(df)
	return df



# go into the folder get all the .txt files
args = parse_arguments()
date = args.date
date = date[2:]
print(date)
basePath = f'{args.input_files}/MiscData/TScan/{date}/SF/'
pdfPath = f'{args.input_files}/DataAnalysis/TScanProcessing/Output/TScans_20{date}.pdf'


# make a list out of all the folders
folders = list_folders_in_directory(basePath)
txtfiles = []
for f in folders:
	# get all the txt files paths and make a folder for them 
	txtfiles.append(get_text_files(basePath+f))


markersize = 10
with PdfPages(pdfPath) as pdf:
	fig, ax= plt.subplots()
	for i in range(len(txtfiles)):
		data=get_data(basePath+folders[i]+'/'+txtfiles[i])
		ax.plot(data["Threshold"],data["TriggerRate"],label=f'{txtfiles[i][23:29]}',marker='o',markersize = markersize-i)

	ax.set(xlabel='Threshold', ylabel='Trigger Rate',title=f'Trigger Rate Scans for 20{date}')
	ax.grid()
	ax.legend()
	plt.tight_layout()
	pdf.savefig(fig)
	plt.close(fig)
	#plt.savefig(pdfPath, format="pdf", bbox_inches = "tight")


		



# put them on a pdf 