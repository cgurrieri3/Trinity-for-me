#!/bin/bash

# Define variables
DATABASE="dbSM"
#######################
echo "SiPM Tempatures"
#######################

QUERY="SELECT * FROM SiPM_temps"
OUTPUT_FILE="/data/TrinityLabComputer/Database/ctcpu_state_mess/csv_saves/SiPM_TEMPS.csv"

# Run the InfluxDB command
influx -database "$DATABASE" -execute "$QUERY" -format csv > "$OUTPUT_FILE"

#######################
echo "ASAD Currents"
#######################

QUERY="SELECT * FROM ASADCurrent"
OUTPUT_FILE="/data/TrinityLabComputer/Database/ctcpu_state_mess/csv_saves/ASAD_CURRENTS.csv"

# Run the InfluxDB command
influx -database "$DATABASE" -execute "$QUERY" -format csv > "$OUTPUT_FILE"

###########################
echo "High Voltage Voltage"
###########################

QUERY="SELECT * FROM HV"
OUTPUT_FILE="/data/TrinityLabComputer/Database/ctcpu_state_mess/csv_saves/HV_VOLTAGE.csv"

# Run the InfluxDB command
influx -database "$DATABASE" -execute "$QUERY" -format csv > "$OUTPUT_FILE"

###########################
echo "High Voltage Status"
###########################

QUERY="SELECT * FROM HV_Status"
OUTPUT_FILE="/data/TrinityLabComputer/Database/ctcpu_state_mess/csv_saves/HV_STATUS.csv"

# Run the InfluxDB command
influx -database "$DATABASE" -execute "$QUERY" -format csv > "$OUTPUT_FILE"

#############################
echo "High Voltage Currents"
#############################

QUERY="SELECT * FROM HV_currents"
OUTPUT_FILE="/data/TrinityLabComputer/Database/ctcpu_state_mess/csv_saves/HV_CURRENTS.csv"

# Run the InfluxDB command
influx -database "$DATABASE" -execute "$QUERY" -format csv > "$OUTPUT_FILE"

################################
echo "MUSIC Power Status"
################################

QUERY="SELECT * FROM MUSIC_Power"
OUTPUT_FILE="/data/TrinityLabComputer/Database/ctcpu_state_mess/csv_saves/MUSIC_POWER_STATUS.csv"

# Run the InfluxDB command
influx -database "$DATABASE" -execute "$QUERY" -format csv > "$OUTPUT_FILE"

################################
echo "Modules Status"
################################

QUERY="SELECT * FROM Modules_Status"
OUTPUT_FILE="/data/TrinityLabComputer/Database/ctcpu_state_mess/csv_saves/MODULE_STATUS.csv"

# Run the InfluxDB command
influx -database "$DATABASE" -execute "$QUERY" -format csv > "$OUTPUT_FILE"

################################
echo "SIAB Current"
################################

QUERY="SELECT * FROM SIABcurrent"
OUTPUT_FILE="/data/TrinityLabComputer/Database/ctcpu_state_mess/csv_saves/SIAB_CURRENT.csv"

# Run the InfluxDB command
influx -database "$DATABASE" -execute "$QUERY" -format csv > "$OUTPUT_FILE"

################################
echo "Trigger Board Current"
################################

QUERY="SELECT * FROM TBCurrent"
OUTPUT_FILE="/data/TrinityLabComputer/Database/ctcpu_state_mess/csv_saves/TRIGGER_BOARD_CURRENT.csv"

# Run the InfluxDB command
influx -database "$DATABASE" -execute "$QUERY" -format csv > "$OUTPUT_FILE"

################################
echo "Trigger Rate"
################################

QUERY="SELECT * FROM TriggerRate"
OUTPUT_FILE="/data/TrinityLabComputer/Database/ctcpu_state_mess/csv_saves/TRIGGER_RATE.csv"

# Run the InfluxDB command
influx -database "$DATABASE" -execute "$QUERY" -format csv > "$OUTPUT_FILE"

################################
echo "UC Tempatures"
################################

QUERY="SELECT * FROM UC_temps"
OUTPUT_FILE="/data/TrinityLabComputer/Database/ctcpu_state_mess/csv_saves/MICROCONTROLLER_TEMPATURES.csv"

# Run the InfluxDB command
influx -database "$DATABASE" -execute "$QUERY" -format csv > "$OUTPUT_FILE"




# Get the current date
CURRENT_DATE=$(date +"%Y%m%d")


# Define variables
SOURCE_DIR="/data/TrinityLabComputer/Database/ctcpu_state_mess/csv_saves/"
DESTINATION_USER="sstepanoff3"
DESTINATION_HOST="login-hive-slurm.pace.gatech.edu"
DESTINATION_PATH="/storage/hive/project/phy-otte/shared/Trinity/MiscData/StateMessages/cos4a-10345/$CURRENT_DATE/"
SSH_KEY="/home/sstepanoff3/.ssh/hive"


# Define the full destination path with the current date
#DESTINATION_PATH="$DESTINATION__PATH$CURRENT_DATE/"

# Create the destination directory on the remote server
ssh -i "$SSH_KEY" "$DESTINATION_USER@$DESTINATION_HOST" "mkdir -p '$DESTINATION_PATH'"


# Loop through all .csv files in the source directory
for FILE in "$SOURCE_DIR"*.csv; 
do
    # Check if there are any .csv files to copy
    if [[ -e "$FILE" ]]; then
        echo "Copying $FILE to $DESTINATION_USER@$DESTINATION_HOST:$DESTINATION_PATH"
        scp -i "$SSH_KEY" "$FILE" "$DESTINATION_USER@$DESTINATION_HOST:$DESTINATION_PATH"
    else
        echo "No .csv files found in $SOURCE_DIR"
    fi
done



