#!/bin/bash

# Run the first command
pkill -f influx_load_statemessages.py

# Run the second command
output="/data/TrinityLabComputer/Database/ctcpu_state_mess/influx_load_sm.log"
exec "$(type -p nohup)" /data/TrinityLabComputer/Database/ctcpu_state_mess/influx_load_statemessages.py "${@}" > "$output" 2>&1 &


exit 0
