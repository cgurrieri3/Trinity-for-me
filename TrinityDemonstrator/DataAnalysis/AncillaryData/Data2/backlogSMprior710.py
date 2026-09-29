import warnings
warnings.simplefilter(action='ignore', category=DeprecationWarning)
import pandas as pd
import glob
import os
from functools import reduce
import csv
from datetime import datetime,timezone,timedelta
from pandas.errors import SettingWithCopyWarning

warnings.simplefilter(action="ignore", category=SettingWithCopyWarning)



PATH_TO_FOLDER = "/storage/hive/project/phy-otte/shared/Trinity/MiscData/StateMessages/cos4a-10345/"


ALL_FOLDERS = []
ALL_FOLDERS.append(r"/storage/hive/project/phy-otte/shared/Trinity/MiscData/StateMessages/phys43199/")
ALL_FOLDERS.append(r"/storage/hive/project/phy-otte/shared/Trinity/MiscData/StateMessages/cos4a-10345/INTIAL/")

# get all the dates needed for all the backlog information this is between 20231003-20240710
start_date = "20231003"
#start_date = "20240916"
dates_2_add = 1
FolderDates = []
start_date_formatted = datetime.strptime(start_date, "%Y%m%d").date()
for i in range(0,dates_2_add): 
  day = start_date_formatted + timedelta(days=i)
  folder_date = day.strftime("%Y%m%d")
  FolderDates.append(folder_date)
#print(FolderDates)

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
for i in range(len(all_files)): 
  file_lines = []
  for f in ALL_FOLDERS:
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
  
  df=df.loc[:,~df.columns.str.startswith('metric')]
  df = df.drop_duplicates()
  df.set_index('time', inplace=True)
  #df.index.name = 'time'

  
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

# reformats the df to be better for readiablity and only contain content that is needed
dfm=dfm.loc[:,~dfm.columns.str.startswith('value2')]
final_df=dfm.loc[:,~dfm.columns.str.startswith('name')]

final_df=final_df.dropna(axis=1, thresh=3000)
final_df.fillna(0,inplace=True)
final_df = final_df.sort_index()
print(final_df)
#print(dfm_time)

# will one df make all the csv from the set dates above
for date in FolderDates: 
  csvname = f"/storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/DataCalibration/AncillaryData/Data2/statemessages{date}.csv"
  print(f"\n{csvname}\n")
  year = int(date[:4])
  month = int(date[4:6])
  day = int(date[6:])
  #print(year, day)
  #print(month)
  intialtime = datetime(year, month, day, 0, 0 , 0,0,tzinfo=timezone.utc)
  
  intialtime = intialtime.timestamp()

  
  endtime = intialtime + 86400 # increment one day
  intialtime = intialtime*1000000000
  endtime = endtime*1000000000 # time is in unix so make sure to convert to the same unix nano second time
  #print(intialtime)
  #print(endtime)
  final_df['unixTime'] = final_df.index
  final_df['unixTime'] = pd.to_numeric(final_df['unixTime'],errors='coerce')

# get only the times within that utc data
  filtered_df = final_df[final_df['unixTime'] > intialtime]
  # filtered_df.to_csv(csvname,header=None, sep=',', mode='w')
  filtered_df2 = filtered_df[filtered_df['unixTime'] < endtime]

  # save UTC df to utc csv
  filtered_df2.to_csv(csvname,header=None, sep=',', mode='w')


# End of script
