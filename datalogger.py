import serial
ser = serial.Serial('COM4', 9600, timeout=1)
f=open("C:\\Users\\voge0017\\Desktop\\LOG.txt","a")
f.write("*******************************************************")
while 1: 
    s = ser.readline(90)        
    f.write(s)
    print s

