loglocation = r"C:\Users\Mlah\Downloads" #Path where the log file is saved
savelocation = r"C:\Users\Mlah\Downloads" #Path to create new files

#Names of the files
logname = "LOG12_8_2012"
accelname = "AccelerationData"
gpsname = "GPSData"
barometername = "BarometerData"

logfile = open(r"{0}\{1}.csv".format(savelocation, logname), 'r')
accelData = open(r"{0}\{1}.csv".format(savelocation, accelname), 'w')
gpsData = open(r"{0}\{1}.csv".format(savelocation, gpsname), 'w')
barometerData = open(r"{0}\{1}.csv".format(savelocation, barometername), 'w')

previousData = '' #Stores the keyword for the last data type written to check for dataloss
time = '000000'

print 'Starting to parse.'

for line in logfile:
    data = line.rsplit(',')
    if data[0] == 'Time':
        time = data[1].rstrip()
        
    if data[0] == 'X1':
        accelData.write('\r\nTime,' + time + ',X1,' + data[1].rstrip()) #X1 should always be on a new line with the last time
    if data[0] == 'X2':
        if previousData == 'X1':
            accelData.write(',X2,' + data[1].rstrip())
        else: #Mising data so start a new line and fill in blank columns
            accelData.write('\r\nTime,' + time + ',X1, ,X2,' + data[1].rstrip())
    if data[0] == 'Y2':
        if previousData == 'X2':
            accelData.write(',Y2,' + data[1].rstrip())
        else: #Mising data so start a new line and fill in blank columns
            accelData.write('\r\nTime,' + time + ',X1, ,X2, ,Y2,' + data[1].rstrip())
    if data[0] == 'Z2':
        if previousData == 'Y2':
            accelData.write(',Z2,' + data[1].rstrip())
        else: #Mising data so start a new line and fill in blank columns
            accelData.write('\r\nTime,' + time + ',X1, ,X2, ,Y2, ,Z2,' + data[1].rstrip())

    if data[0] == 'Pressure': #Pressure should always be the start of a new line
        barometerData.write('\r\nTime,' + time + ',Pressure,' + data[1].rstrip())
    if data[0] == 'Temperature':
        if previousData == 'Pressure':
            barometerData.write(',Temperature,' + data[1].rstrip())
        else: #Mising data so start a new line and fill in blank columns
            barometerData.write('\r\nTime,' + time + ',Pressure, ,Temperature,' + data[1].rstrip())
    if data[0] == 'Altitude':
        if previousData == 'Temperature':
            barometerData.write(',Altitude,' + data[1].rstrip())
        else: #Mising data so start a new line and fill in blank columns
            barometerData.write('\r\nTime,' + time + ',Pressure, ,Temperature, ,Altitude,' + data[1].rstrip())

    if data[0] == '$GPGGA':
        gpsData.write('\r\nTime,' + time)
        for i in data:
            gpsData.write(',' + i.rstrip())

    previousData = data[0]

gpsData.close()
barometerData.close()
accelData.close()
logfile.close()
print 'Done parsing.'

    
