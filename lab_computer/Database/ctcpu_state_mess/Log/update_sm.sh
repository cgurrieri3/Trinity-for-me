#!/usr/bin/expect -f


# Configuration for file transfer
set local_file "/data/TrinityLabComputer/Database/ctcpu_state_mess/Log/"
set remote_path "/home/trinity/Programs/Trinity/control_software/Archive/LOGS/SM_LOG.bin"


# Copy the file using scp
spawn rsync -Pau --bwlimit=600 -e "ssh -i /home/sstepanoff3/.ssh/ctcpu" "trinity@172.16.17.82:$remote_path" $local_file

# Close the SSH tunnel
expect eof
