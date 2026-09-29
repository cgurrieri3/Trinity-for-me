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


ALL_FOLDERS = glob.glob(f"{PATH_TO_FOLDER}/*/", recursive=True)
#print(ALL_FOLDERS)

#ALL_FOLDERS.append(r"/storage/hive/project/phy-otte/shared/Trinity/MiscData/StateMessages/phys43199/")
#print(ALL_FOLDERS)
#all_files.remove('/storage/hive/project/phy-otte/shared/Trinity/MiscData/StateMessages/cos4a-10345/INTIAL/MODULE_STATUS.csv')
#all_files.remove('/storage/hive/project/phy-otte/shared/Trinity/MiscData/StateMessages/cos4a-10345/INTIAL/TRIGGER_BOARD_CURRENT.csv')
#all_files.remove('/storage/hive/project/phy-otte/shared/Trinity/MiscData/StateMessages/cos4a-10345/INTIAL/ASAD_CURRENTS.csv')
#all_files.remove('/storage/hive/project/phy-otte/shared/Trinity/MiscData/StateMessages/cos4a-10345/INTIAL/SIAB_CURRENT.csv')


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


for f in ALL_FOLDERS:
  csvname = f"/storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/DataCalibration/AncillaryData/Data2/statemessages{f[81:89]}.csv"
  print(f"\n{csvname}\n")
  year = int(f[81:85])
  month = int(f[85:87])
  day = int(f[87:89])
  #print(year, day)
  #print(month)
  intialtime = datetime(year, month, day, 0, 0 , 0,0,tzinfo=timezone.utc).timestamp()
  #print(intialtime)
  intialtime = intialtime*1000000000


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
    print(df)
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


  dfm=dfm.loc[:,~dfm.columns.str.startswith('value2')]
  #print(dfm)
  final_df=dfm.loc[:,~dfm.columns.str.startswith('name')]
  final_df['unixTime'] = final_df.index
  final_df['unixTime'] = pd.to_numeric(final_df['unixTime'])

  filtered_df = final_df[final_df['unixTime'] > intialtime]
  #print(filtered_df)

  # filtered_df=filtered_df.dropna(axis=1, thresh=3000)
  # filtered_df.fillna(0,inplace=True)
  # filtered_df = filtered_df.sort_index()
  print(filtered_df)
  
  #print(dfm_time)

  filtered_df.to_csv(csvname,header=None, sep=',', mode='w')
  
