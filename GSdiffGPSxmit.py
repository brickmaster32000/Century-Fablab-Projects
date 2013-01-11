import serial, signal, time

baud = 9600

#close ports when interrupt is called
def handler(i,j):
    GPS.close()
    xBee.close()
    print "Closed serial ports, have a good day"

#define interrupt as ctrl-c
signal.signal(signal.SIGINT, handler)

#define ports
GPS = serial.Serial('COM3', baud, timeout=1)
xBee = serial.Serial('COM4', baud, timout=1)

#ask gps for RMC sentence
GPS.print "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29"
GPS.print "$PMTK220,1000*1F"  #at 1Hz

#print whatever comes from the GPS output to the XBee.
xBee.print(GPS.readline())






    

