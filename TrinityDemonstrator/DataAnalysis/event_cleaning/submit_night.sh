#!/bin/bash
# submit_by_date_or_file.sh
# Submits Condor jobs for one or more dates.
# Accepts either a single YYYYMMDD or a file containing multiple dates.
MNTPATH="/home/sofia.stepanoff/TrinityDemonstrator"
OSDF_path="/ospool/uw-shared/projects/GATech_Otte/TrinityDemonstrator"
INPUT=$1

if [ -z "$INPUT" ]; then
  echo "Usage: $0 <YYYYMMDD | date_file>"
  exit 1
fi

SUBMIT_TEMPLATE="condense_SM.submit"
BASE_DIR="$MNTPATH/DataAnalysis/MergedData/Output"
OSDF_BASE_DIR="$OSDF_path/DataAnalysis/MergedData/Output"
# Determine if input is a file or a single date
if [ -f "$INPUT" ]; then
  echo "Reading dates from file: $INPUT"
  DATES=$(grep -v '^#' "$INPUT" | grep -E '^[0-9]{8}$')
else
  # Validate date format
  if [[ ! "$INPUT" =~ ^[0-9]{8}$ ]]; then
    echo "Error: '$INPUT' is not a valid YYYYMMDD or a file."
    exit 1
  fi
  DATES="$INPUT"
fi

##
LOG_DIR="$MNTPATH/DataAnalysis/event_cleaning/.logs"
USER_LOG_DIR="${LOG_DIR}/${USER}"
echo "ensuring user log directory exists: $USER_LOG_DIR"
mkdir -p "$USER_LOG_DIR"
chmod 775 "$USER_LOG_DIR"
##

echo "Using submit template: $SUBMIT_TEMPLATE"
echo "---------------------------------------------"

for DATE in $DATES; do
  # echo "Checking directory: $DATE"
  TARGET_DIR="${OSDF_BASE_DIR}/${DATE}/"
  if [ ! -d "$TARGET_DIR" ]; then
    echo "Directory not found: $TARGET_DIR — skipping."
    continue
  fi

  echo "Processing date: $DATE"

  # CLUSTERPDF="/storage/osg-otte1/shared/TrinityDemonstrator/DataAnalysis/event_cleaning/Output/EventCleanedCluster${DATE}_TC_481_TB_239_NP_481_s_256_FA_800_mp_3_er_100_tr_1.pdf"
  # CLUSTEROOT="/storage/osg-otte1/shared/TrinityDemonstrator/DataAnalysis/event_cleaning/Output/EventCleanedCluster${DATE}_TC_481_TB_239_NP_481_s_256_FA_800_mp_3_er_100_tr_1.root"
  # ## doesn't work for pdf?
  # chmod 774 "$CLUSTERPDF"
  # # says "chmod: cannot access '': No such file or directory"
  # chmod 774 "$CLUSTERROOT"
  
  OUTLIST="$MNTPATH/DataAnalysis/event_cleaning/condor_lists/file_list_${DATE}.txt"

  rm -f "$OUTLIST"
  for f in "$TARGET_DIR"/*.root; do
      echo "${f##*/}" >> "$OUTLIST"
  done
  echo "Copying $OUTLIST to data_lists directory."
  cp "$OUTLIST" $MNTPATH/DataAnalysis/data_lists/.
  echo "Setting permissions for $OUTLIST."
  chmod 774 $MNTPATH/DataAnalysis/data_lists/file_list_${DATE}.txt
  echo "Created $OUTLIST with $(wc -l < "$OUTLIST") files"
  echo "Submitting job for: $BASENAME"
  condor_submit Date="$DATE" "$SUBMIT_TEMPLATE"

  #echo "Submitted all jobs for date $DATE"
  #echo "---------------------------------------------"
done

echo "All submissions complete."
# mkdir -p /storage/osg-otte1/shared/TrinityDemonstrator/DataAnalysis/MergedData/Output/$DATE
# echo "Watcher started..."
# apptainer exec --bind /storage/osg-otte1/shared/TrinityDemonstrator:/mnt \
#   /storage/osg-otte1/shared/TrinityDemonstrator/DataAnalysis/containers/python3_10.sif \
#   python3 /storage/osg-otte1/shared/TrinityDemonstrator/DataAnalysis/MergedData/watcher.py \
#   -i /mnt/DataAnalysis/MergedData/ \
#   -o /mnt/DataAnalysis/MergedData/Output/ \
#   -l /mnt/DataAnalysis/MergedData/.logs/watcher.log \
#   -t 120
# echo "Watcher finished."
exit 1
