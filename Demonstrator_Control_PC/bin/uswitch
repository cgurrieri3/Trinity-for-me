#!/usr/bin/python3

#Import libs
import socket
import sys
import argparse
import time

# DIRECTION RELAY: RELAY0, NC down, NO up
#      PUSH RELAY: RELAY1, NC off , NO engage motor

class DoorControl:
    def __init__(self,ARGS):
        # Define the IP, port, and buffer size for the uswitch device
        self.TCP_IP      = '172.16.17.75'
        self.TCP_PORT    = 9760
        self.BUFFER_SIZE = 1024

        #Parse the args fed to class
        DIRECTION = ARGS.dir
        self.DT = ARGS.dt
        
        if DIRECTION == 'up':
            self.Up()
        elif DIRECTION == 'down':
            self.Down()
        else:
            print("ERROR WRONG DIRECTION: ",DIRECTION,", please type up or down!")
            sys.exit(1)

    def Up(self):
        self.ResetPUSH()
        print('Opening the pod bay door...')
        for i in range(0,2):
            self.OpenSocket('RELAY{0}=1'.format(i))
        self.TimedResetPUSH()
        
    def Down(self):
        self.ResetPUSH()
        print('Closing the pod bay door...')
        for i in range(0,2):
            self.OpenSocket('RELAY{0}={0}'.format(i))
        self.TimedResetPUSH()

    def ResetPUSH(self): 
        print('Resetting PUSH relay...')
        self.OpenSocket('RELAY1=0')

    def TimedResetPUSH(self):
        time.sleep(self.DT)
        self.ResetPUSH()
        
    def OpenSocket(self,MESSAGE):
        print(MESSAGE)
        s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        s.connect((self.TCP_IP,self.TCP_PORT))
        s.setblocking(True)
        s.settimeout(1)
        s.send(MESSAGE.encode())
        try:
            data = s.recv(self.BUFFER_SIZE)
            s.close()
        except:
            s.close()
        

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description = 'Program to control the Trinity Demonstrator shutter door.')
    parser.add_argument('-dir', metavar='DIRECTION', required=True, help = 'Door direction (up/down)')
    parser.add_argument('-dt', metavar= 'PUSH_DURATION', required=False, default=12, type = int, help = "Modify the 'PUSH' duration for the door in seconds. This can be used to partially open/close the door. Defaults to 12 seconds.")
    args = parser.parse_args()

    DoorControl(args)
