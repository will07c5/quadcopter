#! /usr/bin/env python3.2
import sys
import time
import array

import talk_cons
import talk_misc

import pygame

data = array.array('B',[talk_cons.COMMAND, 1,1])

arm_pressed = False
disarm_pressed = False
def process_buttons(my_client, joy):
	global arm_pressed, disarm_pressed
	arm_pressed_old = arm_pressed
	disarm_pressed_old = disarm_pressed
	arm_pressed = joy.get_button(7)
	disarm_pressed = joy.get_button(4)

	if (arm_pressed and not arm_pressed_old):
		my_client.send(talk_misc.packet_pack(talk_cons.COMMAND, talk_cons.ARM))
		print("sent arm")

	if (disarm_pressed and not disarm_pressed_old):
		my_client.send(talk_misc.packet_pack(talk_cons.COMMAND, talk_cons.DISARM))
		print("sent disarm")

def main():
	if(talk_misc.check_version()!=talk_cons.ALL_OK):
		print("We require 3.2")
		return

	f = open("constants.txt")

	constants = []

	for line in f:
		constants.append(float(line))

	print(constants)

	pygame.init()
	pygame.joystick.init()
	
	joy = pygame.joystick.Joystick(2) #XXX bad
	joy.init()

	my_client = talk_misc.client()
	while(my_client.connect()!=talk_cons.ALL_OK):
		time.sleep(0.35) #do something else for 350ms - random number
	print("Connected")
	print("Asked for info!")
	#my_client.send(talk_misc.packet_pack(talk_cons.COMMAND,talk_cons.STOP))

	my_client.send(talk_misc.packet_pack(talk_cons.COMMAND, talk_cons.TUNE, constants, 'f'))
	while(True):
		pygame.event.pump()

		# scale to reasonable angle in radians
		pitch = joy.get_axis(0) * 0.14
		roll = joy.get_axis(1) * 0.14
		yaw = 0.0
		# throttle is from 0 to uint16_max
		throttle = (-joy.get_axis(4) + 1.0)/2.0 * 50000.0 + 5000
		
		my_client.send(talk_misc.packet_pack(talk_cons.COMMAND, talk_cons.CONTROL, [pitch, roll, yaw, throttle], 'f'))

		process_buttons(my_client, joy)

		my_client.send(talk_misc.packet_pack(talk_cons.GET_INFO,talk_cons.GET_ANGLES))
		
		time.sleep(0.02)
		while (True):
			a = my_client.loop()

			if (a == None):
				break

			if (a[1] == talk_cons.GET_ANGLES):
				print(talk_misc.packet_unpack(a, 'f'))

if __name__ == "__main__":
	main()
