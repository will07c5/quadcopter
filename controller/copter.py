from serial import *
import struct
import sys

class CopterException(Exception): pass

class CopterCommand:
	def __init__(self):
		self.cmd = 0
		self.buf = bytes()

class CopterSensors:
	def __init__(self):
		self.gyroscope = [0, 0, 0]
		self.accelerometer = [0, 0, 0]
		self.magnetometer = [0, 0, 0]

		self.angles = [0.0, 0.0, 0.0]

class Copter:
	def __init__(self, port_path):
		self.connected = False
		self.sensors = CopterSensors()

		try:
			self.port = Serial(port=sys.argv[1], baudrate=115200, timeout=1, writeTimeout=1)
		except SerialException, e:
			raise CopterException("Failed to open serial port.");

	def sendCommand(self, cmd, buf=[]):
		outbuf = bytearray()
		length = len(buf)
		outbuf.append(length)
		outbuf.append(cmd)
		outbuf.extend(buf)

		try:
			self.port.write(outbuf)
		except SerialTimeoutException:
			raise CopterException("Send timed out.")

	def recvCommand(self):
		if (self.port.inWaiting() == 0):
			return None
		
		cmd = CopterCommand()


		try:
			sizebytes = self.port.read()
			size = int(ord(sizebytes[0]))
			cmd.cmd = self.port.read()
			cmd.buf = self.port.read(size)
		except SerialTimeoutException:
			raise CopterException("Receive timed out.")

		return cmd

	def processCommand(self):
		cmd = self.recvCommand()

		if (cmd == None):
			return

		if (cmd.cmd == "c"):
			self.connected = True
		elif (cmd.cmd == "d"):
			self.connected = False
		elif (cmd.cmd == '0'): # sensor data
			sd = struct.unpack("fffffffff", cmd.buf)

			self.sensors.gyroscope = [ sd[0], sd[1], sd[2] ]
			self.sensors.accelerometer = [ sd[3], sd[4], sd[5] ]
			self.sensors.magnetometer = [ sd[6], sd[7], sd[8] ]
		elif (cmd.cmd == '1'): # sensor fusion
			sd = struct.unpack("fff", cmd.buf)
			self.sensors.angles = [ sd[0], sd[1], sd[2] ]


	def updateGains(self, gain1, gain2):
		self.sendCommand('u', struct.pack("ff", gain1, gain2))

	def sendConnect(self):
		self.sendCommand('c', [])

	def sendDisconnect(self):
		self.sendCommand('d', [])





