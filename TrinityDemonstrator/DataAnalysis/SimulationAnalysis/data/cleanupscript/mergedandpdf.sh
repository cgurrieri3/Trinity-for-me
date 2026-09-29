#!/usr/bin/env bash
set -euo pipefail
 
# Extracts date from the current directory name (expects YYYYMMDD)
basepath="/cedar/DataAnalysis/SimulationAnalysis/data"
DATE=$1
cd "${basepath}/${DATE}" || { echo "Directory ${basepath}/${DATE} not found."; exit 1; }
echo " Running for Sims in folder: ${DATE}"
 
echo "Merging DataFiles_*.root -> ${DATE}_datafiles.root"
hadd -f "SIM_DataFiles_${DATE}.root" datafiles_*.root
 
echo "Merging plots_*.root -> ${DATE}_plots.root"
hadd -f "SIM_Plots_${DATE}.root" plots_*.root

echo "create pdf from root file"
cd ${basepath}/cleanupscript
./convertRoottoPDF "${basepath}/${DATE}/SIM_Plots_${DATE}.root"
echo "Done. Output: ${DATE}_plots.pdf"

cd "${basepath}/${DATE}" || { echo "Directory ${basepath}/${DATE} not found."; exit 1; }
mkdir data_files
mkdir plot_files
mv datafiles_*.root data_files/.
mv plots_*.root plot_files/.
 
