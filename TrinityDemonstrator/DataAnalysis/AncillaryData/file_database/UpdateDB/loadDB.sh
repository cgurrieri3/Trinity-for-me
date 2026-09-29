#!/bin/bash
# Check if the hostname argument is provided
if [ $# -eq 0 ]; then
  echo "Usage: $0 <computer-node> <YYYYMMDD>"
  exit 1
fi

# Set the compute node from the first argument
computer_node="$1"
Date="$2"
database="$3"
ssh_command="ssh $computer_node"

# Run commands on the remote host
$ssh_command <<EOF
cd /storage/hive/project/phy-otte/shared/database_rh9
mysql $database < /storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/DataCalibration/DatabaseUpload/scripts/Output/Celestial_$Date.sql
echo Completed: Celestial_$Date.sql
mysql $database < /storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/DataCalibration/DatabaseUpload/scripts/Output/Ranking_$Date.sql
echo Completed: Ranking_$Date.sql
mysql $database < /storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/DataCalibration/DatabaseUpload/scripts/Output/Weather_$Date.sql
echo Completed: Weather_$Date.sql
mysql $database < /storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/DataCalibration/DatabaseUpload/scripts/Output/DataInfo_$Date.sql
echo Completed: DataInfo_$Date.sql
EOF