import serial, signal

def handler(i,j):
    ser.close()
    f.close()
    print "Closed serial port and file, have a good day"

signal.signal(signal.SIGINT, handler)
ser = serial.Serial('COM3', 9600, timeout=1)
f=open("C:\\Users\\Jacob\\Desktop\\LOG.txt","a")
f.write("*******************************************************\n")
while 1: 
    s = ser.readline(90)        
    f.write(s)
    print s

