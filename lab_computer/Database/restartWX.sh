#!/bin/bash

# Run the first command
pkill -f influxdb_load_cont.py

# Run the second command
output="/data/TrinityLabComputer/Database/weather_db/influxdb_load_cont.log"
exec "$(type -p nohup)" /data/TrinityLabComputer/Database/weather_db/influxdb_load_cont.py "${@}" > "$output" 2>&1 &


exit 0
