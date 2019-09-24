#!/usr/bin/env python

import sys
from copter import *


if (len(sys.argv) < 3):
	print("Usage: set_motor.py <motor> <val>")
	sys.exit(1)

try:
	quad = Copter()

	quad.setMotor(int(sys.argv[1]), int(sys.argv[2]))
except CopterException as e:
	print(e)
	sys.exit(1)

