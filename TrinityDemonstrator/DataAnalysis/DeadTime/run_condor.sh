#!/bin/bash
# run_condor.sh

source /opt/root/bin/thisroot.sh

# Setup ExACT
export EXACT_DIR=/exact
export LD_LIBRARY_PATH=/exact/dict:/usr/lib/oracle/21/client64/lib:/usr/local/lib:$LD_LIBRARY_PATH


DATE=$1 
# FILENAME=$2

echo "Running job for $DATE with file "
# $FILENAME
echo "Directories"
ls -lh
# Create necessary directories if they don't exist
mkdir -p DataAnalysis/MergedData/Output/
# mkdir -p DataAnalysis/MergedData/Output/lsheram6_MergedFiles/
cd DataAnalysis/MergedData/Output/
# cd DataAnalysis/MergedData/Output/lsheram6_MergedFiles/
pwd
ls -lh
cd
# $DATE/
# mkdir -p DataAnalysis/DeadTime/.log/
mkdir -p DataAnalysis/DeadTime/Output/
cd DataAnalysis/DeadTime/Output/
pwd
ls -lh
cd

# add line to create .log/user folder if doesn't exist
# mkdir -p DataAnalysis/DeadTime/.log/$theUser/
# cd DataAnalysis/DeadTime/.log/
# mv log ${theUser}

# mv Merged_$FILENAME DataAnalysis/MergedData/Output/$DATE/

mv $DATE DataAnalysis/MergedData/Output/.
echo "moved DATE to MergedData/Output"
cd DataAnalysis/MergedData/Output/
# mv $DATE DataAnalysis/MergedData/Output/lsheram6_MergedFiles/.
# echo "moved DATE to MergedData/Output/lsheram6_MergedFiles"
# cd DataAnalysis/MergedData/Output/lsheram6_MergedFiles/
pwd
ls -lh
cd $DATE
pwd 
ls -lh
cd
mv DeadTime DataAnalysis/DeadTime/.
echo "moved DeadTime to DeadTime/"
cd DataAnalysis/DeadTime/
pwd 
ls -lh

echo "Running flasher calibration for $DATE and File $FILENAME"
# cd DataAnalysis/DeadTime/

./DeadTime $DATE ~/
# $FILENAME

# cd DataAnalysis/DeadTime/.log/
# mv log ${theUser}_flash

cd 
cd DataAnalysis/DeadTime/Output/
pwd
ls -lh
# touch ${DATE}_DeadTime_complete.txt
cd 
ls -lh
# mv ${DATE}_DeadTime_Factor.root DataAnalysis/DeadTime/Output/

cp -r DataAnalysis/DeadTime/Output .
