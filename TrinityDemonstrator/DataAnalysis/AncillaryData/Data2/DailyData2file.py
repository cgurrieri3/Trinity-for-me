
import warnings
warnings.simplefilter(action='ignore', category=DeprecationWarning)
import pandas as pd
import glob
import os
from functools import reduce
import csv
import argparse
from datetime import datetime,timezone,timedelta
from pandas.errors import SettingWithCopyWarning

warnings.simplefilter(action="ignore", category=SettingWithCopyWarning)


# instead of set dates the user chooses the start date and then how many daus
# after they would like to create the csv for. 

# Initialize parser
parser = argparse.ArgumentParser()
parser.add_argument("-d", "--date", help = "YYYYMMDD")
parser.add_argument("-i", "--intAfter", help = "Dates after intial")
parser.add_argument("-ifile", "--input_files", help = "Misc Data .csv for SM")
parser.add_argument("-ofile","--output_files", help = "output filename")
args = parser.parse_args()

# if args.date:
#     print("Displaying Output as: % s" % args.date)


# STARTING DATE FOR FILE CREATIONS AND FOLLOWING DAYS AFTER 
# start_date = "20240718"
start_date = args.date
after_date = int(args.intAfter)

#PATH_TO_FOLDER = "/storage/hive/project/phy-otte/shared/Trinity/MiscData/StateMessages/cos4a-10345/"
#PATH_TO_FOLDER = f"/mnt/MiscData/StateMessages/cos4a-10345/"
PATH_TO_FOLDER = f"{args.input_files}"

# get all the dates needed for all the backlog information this is between userdate + user increment

ALL_FOLDERS = []
start_date_formatted = datetime.strptime(start_date, "%Y%m%d").date()
current_date = datetime.now().date()
for i in range(0,after_date): 
  day = start_date_formatted +timedelta(days=i)
  if day <= current_date:
    folder_date = day.strftime("%Y%m%d")
    ALL_FOLDERS.append(f"{PATH_TO_FOLDER}{folder_date}/")
  else:
    print(f"Date is in the future {day}")

#print(ALL_FOLDERS)

#ALL_FOLDERS.append(r"/storage/hive/project/phy-otte/shared/Trinity/MiscData/StateMessages/phys43199/")
#print(ALL_FOLDERS)
#all_files.remove('/storage/hive/project/phy-otte/shared/Trinity/MiscData/StateMessages/cos4a-10345/INTIAL/MODULE_STATUS.csv')
#all_files.remove('/storage/hive/project/phy-otte/shared/Trinity/MiscData/StateMessages/cos4a-10345/INTIAL/TRIGGER_BOARD_CURRENT.csv')
#all_files.remove('/storage/hive/project/phy-otte/shared/Trinity/MiscData/StateMessages/cos4a-10345/INTIAL/ASAD_CURRENTS.csv')
#all_files.remove('/storage/hive/project/phy-otte/shared/Trinity/MiscData/StateMessages/cos4a-10345/INTIAL/SIAB_CURRENT.csv')

# This is all the SM infromation that is transfered over from the lab computer influxdb database

all_files = ['TRIGGER_RATE.csv',
              'HV_VOLTAGE.csv',
              'HV_CURRENTS.csv', 
              'MICROCONTROLLER_TEMPATURES.csv',
              'MUSIC_POWER_STATUS.csv',
              'HV_STATUS.csv',
              'SiPM_TEMPS.csv',
              'ASAD_CURRENTS.csv',
              'SIAB_CURRENT.csv',
              'TRIGGER_BOARD_CURRENT.csv'
              ]

# loads all the statemessage data into a single pandas data frame

for f in ALL_FOLDERS:
  #csvname = f"/storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/DataCalibration/AncillaryData/Data2/statemessges{f[81:89]}.csv"
  csvname = f"{args.output_files}"
  print(f"\n{csvname}\n")
  year = int(start_date[0:4])
  month = int(start_date[4:6])
  day = int(start_date[6:8])
  print(year, day)
  print(month)
  intialtime = datetime(year, month, day, 0, 0 , 0,0,tzinfo=timezone.utc).timestamp()
  endtime = datetime(year, month, day, 23, 59 ,59,9999,tzinfo=timezone.utc).timestamp()
  #print(intialtime)
  intialtime = intialtime*1000000000
  endtime = endtime*1000000000
  #print(f'{intialtime:.0f}')
  open(csvname, 'w').close()
  #try: # try except catches files that dont exist so they can be made blank
  for i in range(len(all_files)): 
    file_lines = []
    full_file_paths = f'{f}{all_files[i]}' 
    print(full_file_paths)
    with open(full_file_paths, mode ='r') as file:
        csvFile = csv.reader(file)
        for lines in csvFile:
          file_lines.append(lines)
    
    #print(len(file_lines),len(file_lines[i]))
    df = pd.DataFrame(file_lines)
    #print(df)
    df.columns = df.iloc[0]    
    df = df[1:]    
    #print(df)       
    df=df.loc[:,~df.columns.str.startswith('metric')]
    df = df.drop_duplicates()
    df.set_index('time', inplace=True)
    #df.index.name = 'time'
    #print(df)

    
    # Rename the index to 'time'
    #print(df)
    #print(f'File on {i}')
    if i == 0:
      df_intial = df
      #df_intial = df_intial[~df_intial.index.duplicated(keep='first')]
      #df.rename(columns={'0': 'time'}, inplace=True)
      #df_intial.set_index('time', inplace=True)
      #df_intial.set_index('time', inplace=True)
      #df_intial.index.name = 'time'
    else: 
      ##print('DF_INTIAL')
      #p#rint(df_intial)
      #print(df)
      dfm = pd.concat([df_intial,df],axis=1,join='outer')
      #dfm = dfm[~dfm.index.duplicated(keep='first')]
      #dfm=dfm.drop_duplicates()
      df_intial = dfm
      #df_intial.set_index('time', inplace=True)
      #df_intial.index.name = 'time'
      
      #print(dfm)

  #print("end of it all")
  #print(dfm)


  #print(all_lines)  
  #print(len(all_lines),len(all_lines[0]))
      
      


  #final_df.append( reduce(lambda  left,right: pd.merge(left,right,on=['time'],
      #                                        how='left'), all_df ))
                                              
                                              
                                              
  #final_df=final_df.dropna(axis=1, thresh=5000)

# will one df make all the csv from the set dates above
  
  dfm=dfm.loc[:,~dfm.columns.str.startswith('value2')]
  #print(dfm)
  final_df=dfm.loc[:,~dfm.columns.str.startswith('name')]
  final_df['unixTime'] = final_df.index
  final_df['unixTime'] = pd.to_numeric(final_df['unixTime'])
# get only the times within that utc data
  filtered_df = final_df[final_df['unixTime'] > intialtime]
  filtered_df = filtered_df[filtered_df['unixTime'] < endtime]
  #print(filtered_df)

  # filtered_df=filtered_df.dropna(axis=1, thresh=3000)
  # filtered_df.fillna(0,inplace=True)
  # filtered_df = filtered_df.sort_index()
  print(filtered_df)
  
  #print(dfm_time)
#  save UTC df to utc csv
  filtered_df.to_csv(csvname,header=None, sep=',', mode='w')
  #except:
    
  
#final_df.to_csv(r'/storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/DataCalibration/AncillaryData/Data2/Output.csv',header=None, sep=',', mode='w')

