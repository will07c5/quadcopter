#! /usr/bin/env python3.2
import sys
import time

import talk_cons
import talk_misc
import copter

quad = copter.Copter()

my_server = None

def info_callback(socket,buffer):
	print('info callback')
	if(buffer[1]==talk_cons.GET_ANGLES):
		a = [quad.sensors.angles[0], 
				quad.sensors.angles[1], 
				quad.sensors.angles[2], 
				quad.sensors.motor1,
				quad.sensors.motor2,
				quad.sensors.motor3,
				quad.sensors.motor4]
		socket.sendall(talk_misc.packet_pack(talk_cons.HERE_IS_INFO,talk_cons.GET_ANGLES,a,'f'))
	else:
		print("Help! Unknown info request")
		print(buffer)


def action_callback(socket,buffer):
	cmd = buffer[1]
	if(cmd == talk_cons.STOP):
		print('stopping all')
	elif (cmd == talk_cons.ARM):
		quad.arm()
	elif (cmd == talk_cons.DISARM):
		quad.disarm()
	elif (cmd == talk_cons.CONTROL):
		data = talk_misc.packet_unpack(buffer, 'f')[3]

		throttle = int(data[3])
		if (throttle < 0):
			throttle = 0
		elif (throttle > 65535):
			throttle = 65535


		quad.control(data[0], data[1], data[2], throttle)
	elif (cmd == talk_cons.TUNE):
		data = talk_misc.packet_unpack(buffer, 'f')[3]

		print("updating control gains ", data)

		quad.updateGains(data[0], data[1])
		quad.tune(data[2], data[3], data[4], data[5], data[6], data[6], data[7], data[8], data[9])
	else:
		print('not sure command')
		print(buffer)
	socket.sendall(talk_misc.packet_pack(talk_cons.COMMAND_ACK,buffer[1]))


def main():
	global my_server
	if(talk_misc.check_version()!=talk_cons.ALL_OK):
		print("We require 3.2")
		return
	my_server = talk_misc.server(action_callback,info_callback)
	while(my_server.accept()!=talk_cons.ALL_OK):
		time.sleep(0.35) #do something else for 350ms - random number
	print("First Server Connected!!")
	quad.sendConnect()
	while(True):
		quad.processCommand()
		my_server.loop()
		my_server.accept()
if __name__ == "__main__":
	main()
