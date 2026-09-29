#!/bin/bash
# run_condor.sh

source /opt/root/bin/thisroot.sh

# Setup ExACT
export EXACT_DIR=/exact
export LD_LIBRARY_PATH=/exact/dict:/usr/lib/oracle/21/client64/lib:/usr/local/lib:$LD_LIBRARY_PATH


DATE=$1
FILENAME=$2
USER_NAME=$3
echo "Running job for $DATE with file $FILENAME for User $USER_NAME"
echo "Directories"
pwd
ls -lh

# Create necessary directories if they don't exist

mkdir -p DataAnalysis/MergedData/scripts
mkdir -p DataAnalysis/MergedData/{Output,ReducedInfoOutput}/$DATE

cd DataAnalysis/MergedData/scripts/

mkdir MergedData
touch BadFiles.csv
cd $_CONDOR_SCRATCH_DIR
# mkdir -p DataAnalysis/MergedData/Output/$DATE/
# mkdir -p DataAnalysis/MergedData/ReducedInfoOutput/$DATE/
mkdir -p DataAnalysis/exact/data
mkdir -p DataAnalysis/AncillaryData/Data1
mkdir -p DataAnalysis/AncillaryData/Data2
mkdir -p Data/$DATE/RawDataMerged
mkdir -p MiscData/WeatherData/weather
chmod 777 DataAnalysis -R
chmod 777 Data -R
chmod 777 MiscData -R

cd $_CONDOR_SCRATCH_DIR
pwd
mv $FILENAME Data/$DATE/RawDataMerged/.
mv statemessages${DATE}.csv DataAnalysis/AncillaryData/Data2/.
mv celestialPositions${DATE}.csv DataAnalysis/AncillaryData/Data1/.
mv UCTempatureCorrections.csv DataAnalysis/exact/data/.
mv SiPMTempatureCorrections.csv DataAnalysis/exact/data/.
mv weather_${DATE} MiscData/WeatherData/weather/.

ls -lh
cd $_CONDOR_SCRATCH_DIR
cd Data/$DATE/RawDataMerged/
ls -lh
cd $_CONDOR_SCRATCH_DIR
cd DataAnalysis/AncillaryData/Data1/
ls -lh
cd $_CONDOR_SCRATCH_DIR


echo "Running FileMerge for $DATE and File $FILENAME"
# ldd ./FileMerge
./FileMerge $DATE $_CONDOR_SCRATCH_DIR/ $FILENAME
# sleep 10
echo "Running Calibration for $DATE and File $FILENAME"
./AddCalibData $DATE $_CONDOR_SCRATCH_DIR/ Merged_$FILENAME

echo "Running reduced data info for $DATE and File $FILENAME"
./ReduceDataInfoSaved $DATE $_CONDOR_SCRATCH_DIR/ Merged_$FILENAME
ls -lh

cd $_CONDOR_SCRATCH_DIR
cd DataAnalysis/MergedData/Output/$DATE/
chmod 774 Merged_${FILENAME}
cd $_CONDOR_SCRATCH_DIR
cd DataAnalysis/MergedData/ReducedInfoOutput/$DATE/
chmod 774 Merged_${FILENAME}
pwd
cd $_CONDOR_SCRATCH_DIR
ls -lh DataAnalysis/MergedData/ReducedInfoOutput/$DATE/

cd $_CONDOR_SCRATCH_DIR
pwd
ls -lh