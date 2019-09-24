from copter import *
import sys

if (len(sys.argv) < 2):
	print("Usage: echo_test.py <serial device>")
	sys.exit(1)

buf = "Test string".encode('ascii')

c = Copter(sys.argv[1])

c.sendCommand('t', buf)

print "Sent", buf

while True:
	cmd = c.recvCommand()

	if (cmd != None):
		print cmd.buf.decode('ascii')
		break



