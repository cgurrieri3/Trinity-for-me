#!/bin/bash

# CSV file path
csv_file="folders.csv"

# Check if CSV exists
if [ ! -f "$csv_file" ]; then
    echo "Error: $csv_file not found."
    exit 1
fi

# Skip header and read each line
tail -n +2 "$csv_file" | while IFS=, read -r date id_ers; do
    # Trim any extra whitespace
    date=$(echo "$date" | xargs)
    id_ers=$(echo "$id_ers" | xargs)
    cd /projects/cos-lab-aotte6/cos-4a17118/Trinity/TrinityDemonstrator/DataAnalysis/SimulationAnalysis/scripts
    echo "Running ./run_script $date $id_ers"

    ./run_script.sh "$date" "$id_ers"
done
