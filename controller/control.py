#!/usr/bin/env python

import curses
import sys
import time
from copter import *


def main(stdscr, quad, gain1, gain2):
	curses.curs_set(0)
	stdscr.nodelay(1)

	# make sure copter is ready to receive a command
	time.sleep(0.5)

	quad.updateGains(gain1, gain2)
	quad.sendConnect()
	avg_gx = float(0);
	avg_gy = float(0);
	avg_gz = float(0);
	samples = 0;

	while (1):
		quad.processCommand()

		if (quad.connected):
			stdscr.addstr(0, 0, "Connected   ")

			avg_gx += quad.sensors.gyroscope[0];
			avg_gy += quad.sensors.gyroscope[1];
			avg_gz += quad.sensors.gyroscope[2];
			samples += 1;

			stdscr.addstr(1, 0, "GX: %+10.10f" % quad.sensors.gyroscope[0])
			stdscr.addstr(2, 0, "GY: %+10.10f" % quad.sensors.gyroscope[1])
			stdscr.addstr(3, 0, "GZ: %+10.10f" % quad.sensors.gyroscope[2])
			stdscr.addstr(4, 0, "MX: %+10.10f" % quad.sensors.magnetometer[0])
			stdscr.addstr(5, 0, "MY: %+10.10f" % quad.sensors.magnetometer[1])
			stdscr.addstr(6, 0, "MZ: %+10.10f" % quad.sensors.magnetometer[2])
			stdscr.addstr(7, 0, "AX: %+10.10f" % quad.sensors.accelerometer[0])
			stdscr.addstr(8, 0, "AY: %+10.10f" % quad.sensors.accelerometer[1])
			stdscr.addstr(9, 0, "AZ: %+10.10f" % quad.sensors.accelerometer[2])

			stdscr.addstr(11, 0, "X:  %+10.10f" % (quad.sensors.angles[0]*57.295))
			stdscr.addstr(12, 0, "Y:  %+10.10f" % (quad.sensors.angles[1]*57.295))
			stdscr.addstr(13, 0, "Z:  %+10.10f" % (quad.sensors.angles[2]*57.295))

			stdscr.addstr(15, 0, "AVG GX: %+10.10f" % (avg_gx/samples));
			stdscr.addstr(16, 0, "AVG GY: %+10.10f" % (avg_gy/samples));
			stdscr.addstr(17, 0, "AVG GZ: %+10.10f" % (avg_gz/samples));
		else:
			stdscr.addstr(0, 0, "Disconnected")

		stdscr.refresh()

		# process user input
		c = stdscr.getch()

		if (c == ord('q')):
			quad.sendDisconnect()
			sys.exit(0)

		elif (c == ord('p')):
			quad.sendCommand('p')
		elif (c == ord('o')):
			quad.sendCommand('o')

		time.sleep(0.0001)

if (len(sys.argv) < 4):
	print("Usage: control.py <serial device> <gain1> <gain2>")
	sys.exit(1)

try:
	quad = Copter(sys.argv[1])
except CopterException, e:
	print e
	sys.exit(1)

curses.wrapper(main, quad, float(sys.argv[2]), float(sys.argv[3]))
