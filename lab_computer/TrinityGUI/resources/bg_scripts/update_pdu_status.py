#!/usr/bin/env python3
# Import lib
import sys
import os
import time
import re
import pickle
import ast

# Inputs
PORT      = sys.argv[1]
PHYS_USR  = sys.argv[2]
PHYS_PASS = sys.argv[3]
LCF       = ast.literal_eval(sys.argv[4])

cabs = ['wc','rc']
# Construct the rsync command with the --files-from flag and the find command to filter files

while True:
    output1 = []
    output2 = []
    outlet_data = []
    outlet_load = []
    outlet_statuses = {}
    outlet_loads = {}
    wc_status = {}
    wc_loads = {}
    rc_status = {}
    rc_loads = {}
    try:
        for i,cab in enumerate(cabs):
            cmd1 = f'sshpass -p {PHYS_PASS} ssh -p {PORT} {PHYS_USR}@127.0.0.1  \'/home/mpotts32/control_comp/outletcontrol.exp {cab} status\'' if not LCF else f"/home/mpotts32/control_comp/outletcontrol.exp {cab} status"
            cmd2 = f'sshpass -p {PHYS_PASS} ssh -p {PORT} {PHYS_USR}@127.0.0.1  \'/home/mpotts32/control_comp/outletcontrol.exp {cab} load\'' if not LCF else f"/home/mpotts32/control_comp/outletcontrol.exp {cab} load"
            #print(sys.argv[4],LCF,type(LCF),cmd1,cmd2)

            # Execute the commands and grab output
            out1 = os.popen(cmd1)
            output1.append(out1.read())
            out2 = os.popen(cmd2)
            output2.append(out2.read())
            
            # parse out the output
            outlet_data.append(re.findall(r"([1-8])\s\s(\S+)\s+(\w+)\s+", output1[i]))
            outlet_load.append(re.findall(r"([D,P,E][e,o,n]\w*\s\w*)\s*:\s*([\d.]+)", output2[i]))
            
            inner_dict1 = {}
            for outlets in outlet_data[i]:
                inner_dict1[f'{outlets[0]}'] = outlets[2]

            inner_dict2 = {}
            for stats in outlet_load[i]:
                inner_dict2[f'{stats[0]}'] = stats[1]
                
                
            outlet_statuses[cab] = inner_dict1
            outlet_loads[cab] = inner_dict2
            
        # Saving the dictionary as a pickle file
        print("updating outlet_status.pkl...")
        with open("outlet_status.pkl","wb") as f:
            pickle.dump(outlet_statuses,f)
        print("updating outlet_load.pkl...")
        with open("outlet_loads.pkl","wb") as f:
            pickle.dump(outlet_loads,f)

        time.sleep(60)
    except:
        print("Error")
        time.sleep(5)
    
    

