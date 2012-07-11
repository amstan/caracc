#!/usr/bin/env python2

import serial
import struct
import sys

def readnumber(s):
	bytes=s.read(2)
	return struct.unpack(">h",bytes)[0]
	#return reduce(lambda x,y: x*256+ord(y), bytes, 0)

number=0
if __name__=="__main__":
	s=serial.Serial(port=sys.argv[1],baudrate=115200)
	
	while 1:
		#sync
		unsyncdcount=0
		while number!=-32768:
			unsyncdcount+=1
			if unsyncdcount>10:
				unsyncdcount=8
				s.read(1)
				print "skipping byte to sync"
			
			number=readnumber(s)
		
		for i in range(2+4):
			for d in range(3):
				number=readnumber(s)
				print ("%+d" % number),
			#print "  ",
		print