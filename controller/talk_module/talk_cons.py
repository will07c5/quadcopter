#CONSTANTS
SERVER = "10.0.0.1" #"pandaboard_meow" #put in your hostname here
PORT = 9977 #free according to wiki

#return values
NOT_YET =  1
ALL_OK = 0
PYTHON_VERSION_BAD = -1


#min size:3, max size: 2+255
#byte    count: 1        2            3    len   
#             Command   sub_command  len   data  



#API first byte, recv side. TO pandaboard
GET_INFO = 0xA0
COMMAND  = 0xB0

#SUB_COMMANDS INFO
GET_ANGLES = 0x01

#SUB_COMMAND
STOP = 0x01
ARM = 0x02
DISARM = 0x03
CONTROL = 0x04
TUNE = 0x05

#API first byte, response from pandaboard
HERE_IS_INFO = 0xA0
COMMAND_ACK  = 0xB0 
