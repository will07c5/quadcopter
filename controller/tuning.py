#!/usr/bin/env python

import curses
import sys
import time
from copter import *

values = {}
value = float(0)
valStep = 0.01
valueName = ""

def enterValueInputState(stdscr, name):
	global state, valueName, values, value
	valueName = name
	if valueName not in values:
		values[valueName] = float(0)
	state = UserInputState.ValueInput
	value = values[valueName]
	showValueInput(stdscr)
	state = UserInputState.ValueInput

def exitValueInputState(stdscr):
	global state
	values[valueName] = value
	state = UserInputState.Tuning

def showValueInput(stdscr):
	global valueName, value, valStep
	stdscr.addstr(11, 0, "%s: %+10.10f Step: %10.10f" % (valueName, value, valStep))

def handleUserInput(stdscr, c):
	global value, valStep
	if (state == UserInputState.ValueInput):
		if (c == ','):
			value -= valStep
			showValueInput(stdscr)
		elif (c == '.'):
			value += valStep
			showValueInput(stdscr)
		elif (c == '['):
			valStep /= 10
			showValueInput(stdscr)
		elif (c == ']'):
			valStep *= 10
			showValueInput(stdscr)
		elif (c == '\n'):
			exitValueInputState(stdscr)
	elif (state == UserInputState.Tuning):
		elif (c == 'q'):
			quad.sendCommand('d')
			sys.exit(0)

def main(stdscr, quad):
	curses.curs_set(0)
	stdscr.nodelay(1)

	while (1):
		quad.processCommand()

		if (quad.connected):
			stdscr.addstr(0, 0, "Connected   ")

			stdscr.addstr(1, 0, "GX: %+10d" % quad.sensors.gyroscope[0])
			stdscr.addstr(2, 0, "GY: %+10d" % quad.sensors.gyroscope[1])
			stdscr.addstr(3, 0, "GZ: %+10d" % quad.sensors.gyroscope[2])
			stdscr.addstr(4, 0, "MX: %+10d" % quad.sensors.magnetometer[0])
			stdscr.addstr(5, 0, "MY: %+10d" % quad.sensors.magnetometer[1])
			stdscr.addstr(6, 0, "MZ: %+10d" % quad.sensors.magnetometer[2])
			stdscr.addstr(7, 0, "AX: %+10d" % quad.sensors.accelerometer[0])
			stdscr.addstr(8, 0, "AY: %+10d" % quad.sensors.accelerometer[1])
			stdscr.addstr(9, 0, "AZ: %+10d" % quad.sensors.accelerometer[2])
		else:
			stdscr.addstr(0, 0, "Disconnected")

		stdscr.refresh()

		# process user input
		c = stdscr.getch()

		if (c != -1):
			handleUserInput(stdscr, chr(c))

		time.sleep(0.0001)

if (len(sys.argv) < 2):
	print("Usage: control.py <serial device>")
	sys.exit(1)

try:
	quad = Copter(sys.argv[1])
except CopterException, e:
	print e
	sys.exit(1)

curses.wrapper(main, quad)
