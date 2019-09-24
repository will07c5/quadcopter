#!/usr/bin/env python

import serial
import math
from Queue import *

imu_data = Queue()

class Vector3:
	def __init__(self):
		self.x = 0
		self.y = 0
		self.z = 0

	def __init__(self, x, y, z):
		self.x = x
		self.y = y
		self.z = z

	def dot(self, other):
		return self.x*other.x + self.y*other.y + self.z*other.z

	def cross(self, other):
		return Vector(self.y*other.z - self.z*other.y, 
				self.z*other.x - self.z*other.z, 
				self.x*other.y - self.y*other.x)
	
	def normalize(self):
		return self / math.sqrt(self.x*self.x + self.y*self.y + self.z*self.z)

	def __add__(self, other):
		return Vector(self.x + other.x, self.y + other.y, self.z + other.z)

	def __sub__(self, other):
		return Vector(self.x - other.x, self.y - other.y, self.z - other.z)

	def __div__(self, other):
		return Vector(self.x / other, self.y / other, self.z / other)

def open_serial(): 
	try:
		port = serial.Serial("/dev/tty.usbmodemfd541", 115200)

		return port
	except serial.SerialException:
		print "Failed to open serial port."
		quit()

def read_from_serial():
	while (port.inWaiting() > 0):
		data = port.read()
		print data.encode("hex")

def do_quest():
	pass


port = open_serial()
port.write("s");

while (True):
	read_from_serial()



