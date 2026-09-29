#!/bin/bash

# Check if the hostname argument is provided
if [ $# -eq 0 ]; then
  echo "Usage: $0 <computer-node>"
  exit 1
fi

# Set the compute node from the first argument
computer_node="$1"


ssh_command="ssh $computer_node"


# Run commands on the remote host
$ssh_command <<EOF
hostname
module unload anaconda3
mysqld_safe &
EOF