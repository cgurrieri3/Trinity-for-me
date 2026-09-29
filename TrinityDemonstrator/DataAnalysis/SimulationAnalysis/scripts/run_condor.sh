#!/bin/bash
# run_condor.sh

source /opt/root/bin/thisroot.sh

# Setup ExACT
export EXACT_DIR=/exact
export LD_LIBRARY_PATH=/exact/dict:/usr/lib/oracle/21/client64/lib:/usr/local/lib:$LD_LIBRARY_PATH


DATE=$1
FILENAME=$2
theUser=$3
echo "Running job for $DATE with file $FILENAME "
echo "Directories"
ls -lh

# Create necessary directories if they don't exist
mkdir -p DataAnalysis/SimulationAnalysis/scripts
mkdir -p DataAnalysis/SimulationAnalysis/data
mkdir -p simdata/roots/diffuse/${DATE}_${FILENAME}/GROPT/Tilt_91.560000
ls -lh

mv SimRootFiles DataAnalysis/SimulationAnalysis/scripts/.
mv Tilt_91.560000/ simdata/roots/diffuse/${DATE}_${FILENAME}/GROPT/.
cd simdata/roots/diffuse/${DATE}_${FILENAME}/GROPT/Tilt_91.560000
ls
cd 
ls -l simdata/roots/diffuse/${DATE}_${FILENAME}/GROPT/Tilt_91.560000/*.root
ls -lh
cd DataAnalysis/SimulationAnalysis/scripts/
echo "Running SimRootData for $DATE and File $FILENAME"
# ldd ./FileMerge
./SimRootFiles $DATE $FILENAME /srv/ 
# sleep 10
ls -lh

cd
cd DataAnalysis/SimulationAnalysis/data
mkdir -p ${theUser}_data
mv * ${theUser}_data/.
cd ${theUser}_data
chmod 774 *.root
pwd
ls -lh

cd
pwd
ls -lh