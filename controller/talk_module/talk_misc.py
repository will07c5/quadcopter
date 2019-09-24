import sys
import socket
import struct 
import array
from collections import deque

import talk_cons

data_sizes = {
		'c':1,
		'b':1,
		'B':1,
		'?':1,
		'h':2,
		'H':2,
		'i':4,
		'I':4,
		'l':4,
		'L':4,
		'q':8,
		'Q':8,
		'f':4,
		'd':8}

def packet_pack(command,sub_command,data=None,data_type=None):
	a = array.array('B')
	a.append(command)
	a.append(sub_command)
	a.append(0) #size
	if(data==None):
		a[2]=0
		return a
	for f in data:
		a.extend(struct.pack(data_type,f))
	a[2] = len(a)-3
	if(len(a)>258):
		raise Exception('packet too big')
	return a

def packet_unpack(packet,data_type):
	if(data_type not in data_sizes):
		raise Exception('Bad data type passed')

	data_size = data_sizes[data_type]
	command = packet[0]
	sub_command = packet[1]
	length = packet[2]
	
	if(length==0):
		return (command,sub_command,length,None)
	data = []
	for f in range(3, length+3,data_size):
		one_data = bytes(packet[f:f+data_size])
		data.append(struct.unpack(data_type,one_data)[0])
	return (command,length,sub_command,data)


class client:
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	buff = deque()	
	def __init__(self):
		self.sock.settimeout(None)
		self.sock.setblocking(0)
	def connect(self):
		try:
			self.sock.connect((talk_cons.SERVER,talk_cons.PORT))
			return talk_cons.ALL_OK
		except:
			return talk_cons.NOT_YET
	def send(self, data):
		self.sock.sendall(data)
	def loop(self):
		try:
			data = self.sock.recv(4096)
		except(socket.error):
			return None
		for one_byte in data:
			self.buff.append(one_byte)
		#first we check if we have first two bytes.
		#thne we check if we have the whole message done. if so, tell recv
		if(len(self.buff) >=3 and self.buff[2]+3<= len(self.buff)):
			new_array = []
			#range = xrange in py3.2
			for count in range(self.buff[2]+3):
				new_array.append(self.buff.popleft())
			return new_array
		return None #nothing

	def __del__(self):
		self.sock.close()	

class server:
	
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	q = []
	api_call = None
	info_call = None	
	
	def __init__(self,api_callback,info_callback):
		self.api_call = api_callback
		self.info_call = info_callback
		self.sock.settimeout(0.0)
		self.sock.bind((talk_cons.SERVER,talk_cons.PORT))
		self.sock.listen(1) #max connection = 1
		self.sock.setblocking(0)
	def accept(self):
		try:
			conn, ip_addr = self.sock.accept()
			self.q.append((conn,ip_addr,deque()))
			return talk_cons.ALL_OK
		except:
			return talk_cons.NOT_YET
	def loop(self):
		for one_socket,one_ip,buff in self.q:
			try:
				data = one_socket.recv(4096)
			except(socket.error):
				continue

			for one_byte in data:
				buff.append(one_byte)
			#first we check if we have first two bytes.
			#thne we check if we have the whole message done. if so, launch callback.
			
			while(len(buff) >=3 and buff[2]+3<= len(buff)):
				new_array = []
				#range = xrange in py3.2
				for count in range(buff[2]+3):
					new_array.append(buff.popleft())
				if(new_array[0]==talk_cons.GET_INFO):
					self.info_call(one_socket,new_array)
				elif(new_array[0] == talk_cons.COMMAND):
					self.api_call(one_socket,new_array)
				else: 
					raise Exception("Unknown call")
	
	def __del__(self): #not guaranteed, but eh
		for one_socket,one_ip,buff in self.q:
			one_socket.close()

#misc stuff
def check_version():
	if(sys.version_info <(3,2)):
		return talk_cons.PYTHON_VERSION_BAD
	else:
		return talk_cons.ALL_OK


