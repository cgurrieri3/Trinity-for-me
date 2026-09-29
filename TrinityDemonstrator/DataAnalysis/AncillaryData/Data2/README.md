This is the header for the ancillary data2:
Timestamp TriggerRate HV1 HV2 HV3 HV4 HVCur1 HVCur2 HVCur3 HVCur4 UCTemp#1 UCTemp#10 UCTemp11 UCTemp#12 UCTemp#13 UCTemp#14 UCTemp#15 UCTemp#16 UCTemp#2 UCTemp#3 UCTemp#4 UCTemp#5 UCTemp#6 UCTemp#7 UCTemp#8 UCTemp#9 MUSICMPWR1 MUSICPWR10 MUSICPWR11 MUSICPWR12 MUSICPWR13 MUSICPWR14 MUSICPWR15 MUSICPWR16 MUSICPWR2 MUSICPWR3 MUSICPWR4 MUSICPWR5 MUSICPWR6 MUSICPWR7 MUSICPWR8 MUSICPWR9 HVSW1 HVSW10 HVSW11 HVSW12 HVSW13 HVSW14 HVSW15 HVSW16 HVSW2 HVSW3 HVSW4 HVSW5 HVSW6 HVSW7 HVSW8 HVSW9 SiPMTemp#1 SiPMTemp#10 SiPMTemp#11 SiPMTemp#12 SiPMTemp#13 SiPMTemp#14 SiPMTemp#15 SiPMTemp#16 SiPMTemp#2 SiPMTemp#3 SiPMTemp#4 SiPMTemp#5 SiPMTemp#6 SiPMTemp#7 SiPMTemp#8 SiPMTemp#9 ASADCurrent SIABcurr#1 SIABcurr#10 SIABcurr#11 SIABcurr#12 SIABcurr#13 SIABcurr#14 SIABcurr#15 SIABcurr#16 SIABcurr#2 SIABcurr#3 SIABcurr#4 SIABcurr#5 SIABcurr#6 SIABcurr#7 SIABcurr#8 SIABcurr#9 TBCurr Timestamp
0         1           2   3   4   5   6      7      8      9      10       11        12       13        14        15        16        17        18       19       20       21       22       23       24      25        26        27         28         29         30         31         32         33          34        35        36        37        38        39        40        41        42    43     44     45     46     47     48     49     50    51    52    53    54    55    56    57    57         59          60          61          62          63          64          65          66         67         68         69         70         71         72         73         74          75         76          77          78          79          80          81          82          83         84         85         86         87         88         89          90        91         92
HV = High Voltage Voltage aka from the LVPS board
HVCur = HV Currents per LVPS channel
UCTemp = MicroCrontrol Tempatures
MUSICPWR = Music Chip power status
HVSW = High Voltage switch per siab
SiPMTemps = SiPM Tempatures
ASADCurrent = ASAD current
SIABcurr = SIAB current
TBCurr = Trigger Board Current



Scripts: 
To make all the csv in the correct format and by date I have created some helpful scripts from the files. You only need 2 and the other 2 work but are kind of useless. 

------------------------------ Buzzard (contianers) -----------------------------------------------------------
To run with the container localy use
./run_script.sh YYYYMMDD
    This will use a container and mount the system directories to locate and create the statemessage files
    Uses: DailyData2file.py
    Container: python3_10.sef
To run with HTcondor as a job
condor_submit -a "Date=YYYYMMDD" condense_SM.submit
    This will use a HTCondor job to use the container and run the script on the HTC. 
    Sumbit Script: condense_SM.submit
    Uses: run_condor.sh, DailyData2file.py
    Container: python3_10.sef 

------------------------------------------------------------- HIVE ---------------------------------------------
Useful scipts: 
1. backlogSMprior710.py - this script handles all the Statemessages that were saved prior to 20240710. This is important because 20240710 is the date when I set up the automatic file transfers for the statemessges over to the hive computer. Before this there are two large files that have all the SM data 
"/storage/hive/project/phy-otte/shared/Trinity/MiscData/StateMessages/phys43199/"
"/storage/hive/project/phy-otte/shared/Trinity/MiscData/StateMessages/cos4a-10345/INTIAL/"
The script goes between 20231003-20240710 and for every day creates a statmessageYYYYMMDD.csv regardless of if the date had any sm data. If it does have data then all SM that fit within that UTC day will go into that csv file then move on to the next day. all you need to run this scipt is python3 backlogSMprior710.py and all the packages

2. DailyData2file.py - this script is what will be used on the more recent days any data past 20240710 as all the sm data comes in by that day. The scipt also will make the statmessageYYYYMMDD.csv regardless of if the date had any sm data. This is the script that the cos-4a10345.cos.gatech.edu will call to enuse that this is created daily. The scipt will ensure that all statemessages from a given utc date end up in the appropiate statmessageYYYYMMDD.csv. This script can be run by python3 DailyData2file.py -d YYYYMMDD -i #
-d stands for data and this is the data you want to start at
-i is how many dates you want to do after that and is an int

The more deprecated scripts: 
1. makeData2file.py- this script just takes every sm and adds it to one file but output.csv file 
note: This works best when you removed the first 2 lines from the Output.csv files when you move it to the Data2.csv file
2. makeAllData2file.py - this scipt reads in the folder where the statemessage get placed from the scp scripts that transfers them from the lab computere however this doesnt create blank files and if relatively redundent in functionality to the DailyData2file.py script
