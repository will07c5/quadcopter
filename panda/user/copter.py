import struct
import sys

class CopterException(Exception): pass

class CopterCommand:
	def __init__(self):
		self.cmd = 0
		self.buf = bytearray()

class CopterSensors:
	def __init__(self):
		self.gyroscope = [0, 0, 0]
		self.accelerometer = [0, 0, 0]
		self.magnetometer = [0, 0, 0]

		self.angles = [0.0, 0.0, 0.0]

		self.motor1 = 0.0
		self.motor2 = 0.0
		self.motor3 = 0.0
		self.motor4 = 0.0

		self.p = 0.0
		self.i = 0.0
		self.d = 0.0

class Copter:
	def __init__(self):
		self.connected = False
		self.sensors = CopterSensors()

		self.port_read = open("/dev/quadcopter-ctrl", "rb")
		self.port_write = open("/dev/quadcopter-ctrl", "wb")

	def sendCommand(self, cmd, buf=[]):
		outbuf = bytearray()
		length = len(buf)
		outbuf.append(length)
		outbuf.append(ord(cmd))
		outbuf.extend(buf)

		self.port_write.write(outbuf)
		self.port_write.flush()

	def recvCommand(self):
		cmd = CopterCommand()

		sizebytes = self.port_read.read(1)
		if (len(sizebytes) == 0):
			return None

		size = sizebytes[0]

		while True:
			cmdbytes = self.port_read.read(1)

			if (len(cmdbytes) > 0):
				break;

		cmd.cmd = chr(cmdbytes[0])

		sizeleft = size
		cmd.buf = bytearray()
		while True:
			cmd.buf.extend(self.port_read.read(sizeleft))

			if (len(cmd.buf) < size):
				sizeleft = size - len(cmd.buf)
			else:
				break

		return cmd

	def processCommand(self):
		cmd = self.recvCommand()

		if (cmd == None):
			return

		if (cmd.cmd == 'c'):
			self.connected = True
		elif (cmd.cmd == 'd'):
			self.connected = False
		elif (cmd.cmd == '0'): # sensor data
			sd = struct.unpack("=fffffffff", cmd.buf)

			self.sensors.gyroscope = [ sd[0], sd[1], sd[2] ]
			self.sensors.accelerometer = [ sd[3], sd[4], sd[5] ]
			self.sensors.magnetometer = [ sd[6], sd[7], sd[8] ]
		elif (cmd.cmd == '1'): # sensor fusion
			sd = struct.unpack("=fff", cmd.buf)
			self.sensors.angles = [ sd[0], sd[1], sd[2] ]
		elif (cmd.cmd == '2'):
			sd = struct.unpack("=i", cmd.buf)
			self.sensors.motor1 = sd[0]
		elif (cmd.cmd == '3'):
			sd = struct.unpack("=i", cmd.buf)
			self.sensors.motor2 = sd[0]
		elif (cmd.cmd == '4'):
			sd = struct.unpack("=i", cmd.buf)
			self.sensors.motor3 = sd[0]
		elif (cmd.cmd == '5'):
			sd = struct.unpack("=i", cmd.buf)
			self.sensors.motor4 = sd[0]
		elif (cmd.cmd == '6'):
			sd = struct.unpack("=f", cmd.buf)
			self.sensors.p = sd[0]
		elif (cmd.cmd == '7'):
			sd = struct.unpack("=f", cmd.buf)
			self.sensors.i = sd[0]
		elif (cmd.cmd == '8'):
			sd = struct.unpack("=f", cmd.buf)
			self.sensors.d = sd[0]


	def updateGains(self, gain1, gain2):
		self.sendCommand('u', struct.pack("=ff", gain1, gain2))

	def tune(self, pitch_p, pitch_i, pitch_d, roll_p, roll_i, roll_d, yaw_p, yaw_i, yaw_d):
		self.sendCommand('t', struct.pack("=fffffffff", pitch_p, pitch_i, pitch_d, roll_p, roll_i, roll_d, yaw_p, yaw_i, yaw_d))

	def sendConnect(self):
		self.sendCommand('c', [])

	def sendDisconnect(self):
		self.sendCommand('d', [])

	def setMotor(self, motor, val):
		self.sendCommand('m', struct.pack("=BH", motor, val))

	def control(self, pitch, roll, yaw, throttle):
		self.sendCommand('o', struct.pack("=fffH", pitch, roll, yaw, throttle))

	def arm(self):
		self.sendCommand('a')

	def disarm(self):
		self.sendCommand('s')





