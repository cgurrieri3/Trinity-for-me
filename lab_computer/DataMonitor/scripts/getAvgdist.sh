#!/usr/bin/expect -f

set timeout -1

set remote_user "trinity"
set remote_server "172.16.17.82"


# Get the current date in the format yyyymmdd
set current_date [clock format [clock seconds] -format "%Y%m%d"]
set filename "avgDist"

set remote_file "/home/trinity/Documents/exact_output/$filename.png"
set local_file "/data/TrinityLabComputer/DataMonitor/Outputs/$filename$current_date.png"
set labuser $env(USER)


# Start SSH session
spawn ssh -i /home/$labuser/.ssh/ctcpu $remote_user@$remote_server

expect "$ "

# Run the remote script
send "cd /home/trinity/Programs/aliases/ && ./recentAvgDist.sh\r"

# Wait for the script to finish
expect "$ "

# Close the SSH session
send "exit\r"
#spawn ssh -i /home/$labuser/.ssh/ctcpu $remote_user@$remote_server "cd $remote_program_path" && "$remote_program"






spawn scp -i /home/$labuser/.ssh/ctcpu $remote_user@$remote_server:$remote_file $local_file

# Expect the end of the scp process (you can adjust this to look for specific output)
expect {
    "100%" {
        # Transfer completed
        send_user "\nTransfer complete\n"
    }
    timeout {
        # Handle timeout
        send_user "\nSCP command timed out\n"
        exit 1
    }
}



expect eof
