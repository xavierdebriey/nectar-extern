import time

import serial

command_port = '/dev/tty.usbserial'

ser = serial.Serial(command_port, timeout=3)

def send_recv(data, num=64):
	ser.write(data)
	print('send')
	return ser.read(num)

def hard_reset():
	print(send_recv("+++".encode('ascii')))
	print(send_recv("ATRE\r".encode('ascii')))
	print(send_recv("ATAC\r".encode('ascii')))
	print(send_recv("ATCN\r".encode('ascii')))

def check_ssid():
	print(send_recv("+++".encode('ascii')))
	print(send_recv("ATID\r".encode('ascii')))
	print(send_recv("ATCN\r".encode('ascii')))

def check_ip():
	print(send_recv("+++".encode('ascii')))
	print(send_recv("ATDL\r".encode('ascii')))
	print(send_recv("ATMY\r".encode('ascii')))
	print(send_recv("ATMK\r".encode('ascii')))
	print(send_recv("ATPG 192.168.1.100\r".encode('ascii')))
	print(send_recv("ATCN\r".encode('ascii')))

def connect():
	print(send_recv("+++".encode('ascii')))
	print(send_recv("ATIP \001\r".encode('ascii')))
	print(send_recv("ATDL 10.0.1.6\r".encode('ascii'))) # 192.168.1.100
	print(send_recv("ATCN\r".encode('ascii')))

def test_send():
	while 1:
		ser.write("HELLO WORLD!".encode('ascii'))
		time.sleep(5)

def check_softAP():
	print(send_recv("+++".encode('ascii')))
	# print(send_recv(bytearray([ord('A'), ord('T'), ord('C'), ord('E'), ord(' '), 0x01, ord('\r')])))
	# print(send_recv("ATAC\r".encode('ascii')))
	print(send_recv(bytearray([ord('A'), ord('T'), ord('C'), ord('E'), ord('\r')])))
	print(send_recv("ATCN\r".encode('ascii')))

hard_reset()
time.sleep(2)
check_ssid()

# check_softAP()

# check_ip()

# connect()
# time.sleep(5)
# test_send()
