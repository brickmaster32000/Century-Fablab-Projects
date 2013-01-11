// Test code for Adafruit GPS modules using MTK3329/MTK3339 driver
//
// This code turns on the LOCUS built-in datalogger. The datalogger
// turns off when power is lost, so you MUST turn it on every time
// you want to use it!

#include <Adafruit_GPS.h>

// Connect the GPS Power pin to 5V
// Connect the GPS Ground pin to ground
// If using hardware serial:
//   Connect the GPS TX (transmit) pin to Arduino RX1
//   Connect the GPS RX (receive) pin to matching TX1


// Define hardware serial port.
Adafruit_GPS GPS(&Serial1);


void setup()  
{    
  // connect at 115200 so we can read the GPS fast enough and
  // also spit it out
  Serial.begin(115200);
  Serial.println("GPS logging start test!");

  // 9600 NMEA is the default baud rate for MTK - some use 4800
  GPS.begin(9600);
  
  //Tell GPS to send us the GGA string.
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  //Tell GPS to send us data at 5Hz rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_5HZ);

  Serial1.read();

  Serial.println("----------------------------------------------------");
}


void loop()                     // run over and over again
{  
  // If using hardware serial (e.g. Arduino Mega), change this to Serial1, etc.
  if (Serial1.available()) {
    char c = Serial1.read();
      if (c) Serial.print(c);
      // writing direct to UDR0 is much much faster than Serial.print 
      // but only one character can be written at a time.
  }
}
