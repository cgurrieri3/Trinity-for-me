#!/usr/bin/env python3

import subprocess

print("Turn on the chiller")
subprocess.call("pdu rc 1 on", shell = True)

print("Turing on the MicroTSA crate")
subprocess.call("pdu rc 6 on", shell = True)

print("Turing on the Magma PS")
subprocess.call("pdu rc 7 on", shell = True)

print("Completed power on")
