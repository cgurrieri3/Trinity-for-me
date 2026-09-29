#!/usr/bin/env python3
# Import lib
import sys
import os
import time
import ast

# Inputs
PORT      = sys.argv[1]
PHYS_USR  = sys.argv[2]
PHYS_PASS = sys.argv[3]
LWXDIR    = sys.argv[4]
WXDIR     = sys.argv[5]
LCF       = ast.literal_eval(sys.argv[6])


# Construct the rsync command with the --files-from flag and the find command to filter files
#print(sys.argv[6],LCF,type(LCF))
if LCF: # Lab computer
    cmd = f'find {LWXDIR}/*_*[0-9] -type f -mtime -2 -print0'
else: # remote computer
    cmd = f'sshpass -p {PHYS_PASS} ssh -p {PORT} {PHYS_USR}@127.0.0.1  \'find {LWXDIR}/*_*[0-9] -type f -mtime -2 -print0\''
#print(cmd)
while True:
    # Execute the command
    try:
        out = os.popen(cmd)
    
        #Split output into list
        ls = out.read().split('\x00')
        
        # rsync files that were listed
        for f in ls:
            if f == '': # if entry is empty skip it
                continue
            else: # rysnc/cp lastes file
                if LCF: # Lab computer
                    rsync_cmd = f"cp {f} {WXDIR}"
                else: # remote computer
                    rsync_cmd = f"sshpass -p {PHYS_PASS} rsync -qu -e \'ssh -p {PORT}\' {PHYS_USR}@127.0.0.1:{f} {WXDIR}"
                o = os.popen(rsync_cmd)
    except:
        print('Warning - ssh: connect to host 127.0.0.1 port {PORT}: Connection refused')
    time.sleep(60)
