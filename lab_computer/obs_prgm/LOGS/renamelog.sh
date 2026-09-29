
#!/bin/bash

cd /data/TrinityLabComputer/obs_prgm/LOGS || exit 1

#definne the fixed filename
filename="trinity.log"

# Extract the base name without the extension
basename="${filename%.*}"

# Get the current time in the desired format
current_time=$(date +"%Y-%m-%d %H:%M:%S")

# Append the end of file message with the current time
echo "End of file - $current_time" >> "$filename"

# Get the current date in YYYYMMDD format
current_date=$(date +"%Y%m%d")

# Rename the file to mylog_YYYYMMDD.log
mv "$filename" "${basename}_${current_date}.log"

# Create an empty file called mylog.log
touch "$filename"

echo "Start of file - $current_time" >> "$filename"

#echo "File renamed to ${basename}_${current_date}.log and ${filename} created."
