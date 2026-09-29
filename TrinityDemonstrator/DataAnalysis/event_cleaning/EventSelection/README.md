
Event selection used the datafiles from the event cleaning to produce large composite PDFS and root files of all the event clenaing data. This is so that Event Paremeters can be changed such as cuts to L and W without having to re rerun the event cleaning. This is also where door open and door closed data can be sorted by lists that come from the lab computer database. 
First you select the event clenaing data files you would like to run through and then you selected the file list you would like to restrict to. This is really imporant because some nights can have door open and door closed data. 
if dont have a file list just so n and it will allow all the files. arguments come from eventcleaning/Output and where your mount starts

compile with ./compile_script.sh

usage: 3 arguments needed
./run_script.sh mount folder data list
example:
./run_script.sh y sstepanoff3_DataFiles/SS_20260105_2024toEOY2025 DataAnalysis/data_lists/SS_Rank8_FIIES_20260105_202410toEOY2025.csv
./run_script.sh y sstepanoff3_DataFiles/SS_20260105_2024toEOY2025 n

y : will use the standard mount /mnt 

if you want to use a different mnt:
specify in first argument

rootcling -f EventInfoDict.cpp -c EventInfo.h LinkDef.h