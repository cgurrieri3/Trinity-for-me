#!/bin/bash



# Configuration for SSH tunnel
#jump_host="172.16.17.74"          # Jump host (intermediary server)
#jump_user="root"          # Username on the jump host
remote_server="172.16.17.82"  # Remote SSH server
remote_user="trinity"      # Username on the remote server
remote_port=22                 # SSH port on the remote server
local_port=2222                # Local port for the SSH tunnel

# Configuration for file transfer
# local_file="/home/mpotts32/obs_prgm/rcLogs/Tscan" # phys43199
local_file="/data/TrinityLabComputer/obs_prgm/rcLogs/Tscan" #cos-4a10345

remote_path="/home/trinity/control_software//Archive/Tscan/$1/SF/$2"

# Copy the file using scp through the SSH tunnel
scp -i $HOME/.ssh/ctcpu "$remote_user@$remote_server:$remote_path"/*.txt "$local_file"


