#!/bin/bash
# run_condor.sh

source /opt/root/bin/thisroot.sh

# Setup ExACT
export EXACT_DIR=/exact
export LD_LIBRARY_PATH=/exact/dict:/usr/lib/oracle/21/client64/lib:/usr/local/lib:$LD_LIBRARY_PATH




DATE=$1
FILENAME=$2
USER_NAME=$3
#Add user info here so that it works correctly as jobs may not run as your user on ospool

echo "Running job for $DATE with file $FILENAME for User $USER_NAME"
# echo "Running job for $DATE with file $FILENAME as user $USER_NAME"

echo "Directories"
ls -lh


# Create necessary directories if they don't exist

mkdir -p DataAnalysis/MergedData/Output/$DATE/
mkdir -p DataAnalysis/event_cleaning/ClusterCleaning/
mkdir -p DataAnalysis/flasher_calibration/Output/
mkdir -p DataAnalysis/event_cleaning/Output/
# mkdir -p DataAnalysis/event_cleaning/Output/

mv $DATE DataAnalysis/MergedData/Output/
# mv $DATE DataAnalysis/MergedData/Output/.
echo "Moved $DATE to DataAnalysis/MergedData/Output/"
cd DataAnalysis/MergedData/Output/$DATE/
pwd
ls -lh
cd $_CONDOR_SCRATCH_DIR

mv neighbors DataAnalysis/event_cleaning/ClusterCleaning/
echo "Moved neighbors to DataAnalysis/event_cleaning/ClusterCleaning/"
# mv EventInfo.h DataAnalysis/event_cleaning/ClusterCleaning/
# mv EventInfoDict_rdict.pcm DataAnalysis/event_cleaning/ClusterCleaning/
# mv LinkDef.h DataAnalysis/event_cleaning/ClusterCleaning/
# mv EventCleaning DataAnalysis/event_cleaning/ClusterCleaning/
mv ${DATE}_FlasherCalibration_Factor.root DataAnalysis/flasher_calibration/Output/

##
mv EventCleaning DataAnalysis/event_cleaning/ClusterCleaning/EventCleaning
mv EventInfo.h DataAnalysis/event_cleaning/ClusterCleaning/EventInfo.h
mv EventInfoDict_rdict.pcm DataAnalysis/event_cleaning/ClusterCleaning/EventInfoDict_rdict.pcm
mv LinkDef.h DataAnalysis/event_cleaning/ClusterCleaning/LinkDef.h
echo "Running eventcleaning for $DATE and File $FILENAME for User $USER_NAME"
##
cd DataAnalysis/event_cleaning/ClusterCleaning/
ls -lh
chmod +x EventCleaning
echo $_CONDOR_SCRATCH_DIR
./EventCleaning $DATE $_CONDOR_SCRATCH_DIR/ $FILENAME

ls -lh

cd $_CONDOR_SCRATCH_DIR
cd DataAnalysis/event_cleaning/Output/

echo $USER_NAME
mv DataFiles ${USER_NAME}_DataFiles
rm -r DataFiles
cd ${USER_NAME}_DataFiles


pwd
for f in *.root; do
    [ -e "$f" ] || continue  # skip if no .root files
    echo "Processing file: $FILENAME"
    time=$(echo "$FILENAME" | grep -oP '\d{4}-\d{2}-\d{2}T\K\d{2}_\d{2}')
    echo "Time extracted: $time"
    newname="${f%.root}_$time.root"
    echo "Renaming: $f -> $newname"
    mv "$f" "$newname"
done

# time=$(echo "$FILENAME" | grep -oP '\d{4}-\d{2}-\d{2}T\K\d{2}:\d{2}')
# echo "Time extracted: $time"
# newname="${filename%.root}_$time.root"
# mv "$filename" "$newname"
ls -lh
pwd

