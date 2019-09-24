import fcntl
import sys
import time

firm_out = open("/dev/quadcopter-firmware", "wb")
trace = open("/dev/quadcopter-firmware", "r")
ctrl = open("/dev/quadcopter-ctrl")
firm_in = open("m3-image.bin", "rb")

fcntl.ioctl(ctrl.fileno(), (10 << 8) + 1, 0);

firm_out.write(firm_in.read())

firm_out.flush()
firm_out.close()

fcntl.ioctl(ctrl.fileno(), 10 << 8, 0)

while True:
	time.sleep(0.01)
	sys.stdout.write(trace.read())
