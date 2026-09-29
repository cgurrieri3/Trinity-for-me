#!/usr/bin/python3

#Import libs
import socket
import sys

# print help
if len(sys.argv) == 1:
    help_sting = f'''
    Purpose: This script works to control a single relay on uSwitch relay board. This script is mainly for debugging purposes. 
    Usage: relay RELAY#=#
    Example: relay RELAY0=1
    '''
    print(help_sting)
    sys.exit(1)

#Defining vars
TCP_IP = '172.16.17.75'
TCP_PORT = 9760
BUFFER_SIZE = 1024
MESSAGE = sys.argv[1]

print(MESSAGE)
s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
s.connect((TCP_IP,TCP_PORT))
s.setblocking(True)
s.settimeout(1)
s.send(MESSAGE.encode())
try:
    data = s.recv(BUFFER_SIZE)
except:
    s.close()
