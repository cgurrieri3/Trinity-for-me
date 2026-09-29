#!/usr/bin/python3
# -*- coding: utf-8 -*-

from serial import Serial
from time import sleep
from time import time
import datetime
import sys

STATUS_OV = 1
STATUS_OC = 2
STATUS_PB = 4
STATUS_PGM = 8
STATUS_OT = 16
STATUS_FUSE = 32
STATUS_ALM = 128
STATUS_ILOC = 256
STATUS_REM = 512

MODE_ROTARY = 0
MODE_KEYPAD = 1
MODE_EXT_PGM = 2
MODE_REMOTE = 3

def on_off(s):
	s = s.strip()
	if s == 'ON' or s == b'1':
		return True
	elif s == 'OF' or s == 'OFF' or s == b'0':
		return False
	else:
		raise Exception(
			'Excepted string parameter "ON" of "OF", but received: "%s"' % s)


class Magna(Serial):
	def __init__(self, port, expected_serial_number=None, log=None):
		super(Magna, self).__init__(port, baudrate=19200, timeout=2.0)
		self.log = log if log else self.magna_log
		self.write('*CLS\r\n'.encode())
		# Magna-Power Electronics Inc., XR16-375, S/N: 1162-0361, F/W:1.0
		self.write('*IDN?\r\n'.encode())
		idn = self.magna_readline()
		if not idn.startswith('Magna'.encode()):
			raise Exception('Unexpected IDN: %s' % idn)
		#self.log(idn)
		idnlist = idn.split(', '.encode())
		self.manufacturer = idnlist[0]
		self.model_number = idnlist[1]
		self.serial_number = idnlist[2]
		if self.serial_number.startswith('S/N:'.encode()):
			self.serial_number = self.serial_number[4:]
		else:
			raise Exception(
				'ERROR: Magna serial number is expected to start with "S/N:",'
				' but received: %s' % self.serial_number)
		if expected_serial_number and \
			self.serial_number != expected_serial_number:
			raise Exception(
				'ERROR: Magna serial number is %s, when expected %s on port %s'
				% (self.serial_number, expected_serial_number, port))

	def write(self, command):
		self.log('MAGNA < %s' % command)
		super(Magna, self).write(command)

	def magna_append(self, msg):
		with open('magna.log', 'a') as f:
			f.write(msg)

	def magna_log(self, msg):
		lines = msg.strip().splitlines(False)
		for line in lines:
			line = line.strip()
			if len(line) > 0:  # empty line is never logged
				outstr = datetime.datetime.now().strftime("%Y%m%d_%H:%M:%S.%f")
				outstr += ' ' + line + '\n'
				sys.stderr.write(outstr)
				self.magna_append(outstr)

	def ask(self, question):
		self.write(question)
		return self.magna_readline()

	def magna_readline(self):
		retval = self.readline()
		self.log('MAGNA > %s' % retval)
		return retval

	def cls(self):
		self.write('*CLS\r\n'.encode())

	def rst(self):
		self.write('*RST\r\n'.encode())

	def output_protection_clear(self):
		self.write('OUTPUT:PROTECTION:CLEAR\r\n'.encode())

	def set_voltage(self, value=None):
		if value is None:
			return float(self.ask('VOLT?\r\n'.encode()))
		else:
			vset = int(value + 0.5)
			self.write('VOLT %d\r\n'.encode() % vset)
			return vset

	def voltage_protection(self, value=None):
		if value is None:
			return float(self.ask('VOLT:PROT?\r\n'.encode()))
		else:
			vset = round(value, 3)
			self.write('VOLT:PROT %.3f\r\n'.encode() % vset)
			return vset

	def set_current(self, value=None):
		if value is None:
			return float(self.ask('CURR?\r\n'.encode()))
		else:
			iset = round(value, 3)
			self.write('CURR %.3f\r\n'.encode() % iset)
			return iset

	def current_protection(self, value=None):
		if value is None:
			self.flushInput()
			self.write('CURR:PROT?\r\n'.encode())
			return float(self.readline())
		else:
			cpset = round(value, 3)
			self.write('CURR:PROT:LEV %.4f\r\n'.encode() % cpset)
			return cpset

	def meas_voltage(self):
		return float(self.ask('MEAS:VOLT?\r\n'.encode()))

	def meas_current(self):
		return float(self.ask('MEAS:CURR?\r\n'.encode()))

	def output_state(self, enable=None):
		if enable is None:
			return on_off(self.ask('OUTP:STAT?\r\n'.encode()))
		else:
			self.write(
				'OUTP:START\r\n'.encode() if enable else 'OUTP:STOP\r\n'.encode())
			return enable

	def config_interlock(self, enable=None):
		if enable is None:
			return on_off(self.ask('CONF:INTE?\r\n'.encode()))
		else:
			self.write(
				'CONF:INTE 1\r\n'.encode() if enable else 'CONF:INTE 0\r\n'.encode())
			return enable

	def status_questionable_condition(self):
		sqc = self.ask('STAT:QUES:COND?\r\n'.encode())
		self.log('sqc = %s' % sqc)
		return int(self.ask('STAT:QUES:COND?\r\n'.encode()))

	def status_iloc(self):
		return (self.status_questionable_condition() &
			STATUS_ILOC) == STATUS_ILOC

	def status_operation_condition(self):
		return int(self.ask('STAT:OPER:COND?\r\n'.encode()))

	def wait_to_settle(self, timeout):
		starttime = time()
		close_enough_cnt = 0
		vset = self.set_voltage()
		while True:
			vmeas = self.meas_voltage()
			#self.log('vmeas = %.3f' % vmeas)
			diff_abs = vmeas - vset
			diff_rel = float(diff_abs) / float(vset)
			#print 'diff_abs =', diff_abs, ', diff_rel =', diff_rel
			close_enough = (diff_abs >= -3 and diff_abs <= 3) or \
				(diff_rel >= -0.03 and diff_rel <= 0.03)
			close_enough_cnt = close_enough_cnt + 1 if close_enough else 0
			elapsed = time() - starttime
			if close_enough_cnt > 3:
				self.log('settling_time = %.3f' % elapsed)
				break
			if elapsed > timeout:
				raise Exception('ERROR: timeout waiting from Magna to settle')
			sleep(0.1)

	def operational_mode(self, value=None):
		if value is None:
			return int(self.ask('CONF:SETPT?\r\n'.encode()))
		else:
			self.write('CONF:SETPT %d\r\n'.encode() % value)
			return value

	def remote_sense(self, value=None):
		if value is None:
			return int(self.ask('REM:SENS?\r\n'.encode()))
		else:
			if value:
				self.write('REM:SENS 1\r\n'.encode())
			else:
				self.write('REM:SENS 0\r\n'.encode())

if __name__ == "__main__":
	#usb_serial_number = 'FTFJXVY9'
	#from ttyusb import find_tty_usb_sn
	#port = Serial('/dev/ttyS0')
	port = '/dev/ttyS0'
	m = Magna(port=port)
	m.log('serial_number = %s' % m.serial_number)
	try:
		m.operational_mode(MODE_ROTARY)
		m.log('operational_mode = %d' % m.operational_mode())
		sleep(1)
		m.output_protection_clear()
		sleep(5)
		m.operational_mode(MODE_REMOTE)
		m.log('operational_mode = %d' % m.operational_mode())
		sleep(5)
		#m.rst()
		m.output_state(False)
		m.log('output_state = %d' % m.output_state())
		m.remote_sense(False)
		m.log('remote_sense = %s' % str(m.remote_sense()))
		sleep(2)
		m.voltage_protection(350.0)
		m.log('voltage_protection = %.0f' % m.voltage_protection())
		m.set_voltage(24.0)
		m.log('set_voltage = %.0f' % m.set_voltage())
		m.set_current(2.0)
		m.log('set_current = %s' % str(m.set_current()))
		m.config_interlock(False)
		m.log('config_interlock = %s' % str(m.config_interlock()))
		vmeas = m.meas_voltage()
		m.log('meas_voltage = %.0f' % vmeas)
		m.output_state(True)
		m.log('output_state = %s' % str(m.output_state()))
		m.log('status_iloc = %s' % str(m.status_iloc()))
		m.wait_to_settle(90)
		#m.voltage_protection(550.0)
		m.set_voltage(24.0)
		m.wait_to_settle(90)
		#m.voltage_protection(750.0)
		m.set_voltage(24.0)
		m.wait_to_settle(90)
	finally:
		m.output_state(True)
		m.log('output_state = %s' % str(m.output_state()))
