
#!/bin/bash

# Configuration for SSH tunnel
#jump_host="172.16.17.74"          # Jump host (intermediary server)
#jump_user="root"          # Username on the jump host
remote_server="172.16.17.82"  # Remote SSH server
remote_user="trinity"      # Username on the remote server
remote_port=22                 # SSH port on the remote server
local_port=2222                # Local port for the SSH tunnel

# Configuration for file transfer
local_file="/home/mpotts32/Database/ctcpu_state_mess/Log/"
remote_path="/home/trinity/Programs/Trinity/control_software/Archive/LOGS/SM_LOG.bin"

# Create an SSH tunnel to the remote server through the jump host
# ssh -i ~/.ssh/CTCPU -L "$local_port:$remote_server:$remote_port" "$jump_user@$jump_host" -N &
# tunnel_pid=$!

# Wait for a moment to ensure the tunnel is established
# sleep 2

# Copy the file using scp through the SSH tunnel
scp -i ~/.ssh/trinitySite "$remote_user@$remote_server:$remote_path" "$local_file"

# Close the SSH tunnel
#kill "$tunnel_pid"




# # Define variables
# CTRL_SEVER = "172.16.17.74"
# CTRL_USER = "root"

# TAR_SERVER="172.16.17.82"
# TAR_USER="trinity"

# get_state_messages () {
#     # SCP data
#     scp -i ~/.ssh/ctcpu "$TAR_USER@$TAR_SERVER":/home/trinity/Programs/Trinity/control_software/Archive/LOGS/SM_LOG.bin /home/mpotts32/Database/ctcpu_state_mess/Log/ 
# }

# get_state_messages

# sleep 1

# get_state_messages
