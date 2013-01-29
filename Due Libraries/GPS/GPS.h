/***********************Header Section***********************************************/
#ifndef _GPS_H
#define _GPS_H

#include "Arduino.h"

#define MAXSIZE 90 

#define PMTK_SET_NMEA_UPDATE_1HZ  "$PMTK220,1000*1F"
#define PMTK_SET_NMEA_UPDATE_5HZ  "$PMTK220,200*2C"
#define PMTK_SET_NMEA_UPDATE_10HZ "$PMTK220,100*2F"

#define PMTK_SET_BAUD_57600 "$PMTK251,57600*2C"
#define PMTK_SET_BAUD_9600 "$PMTK251,9600*17"

#define PMTK_SET_NMEA_OUTPUT_RMCONLY "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29"
#define PMTK_SET_NMEA_OUTPUT_GGAONLY "$PMTK314,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29"
#define PMTK_SET_NMEA_OUTPUT_RMCGGA "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"
#define PMTK_SET_NMEA_OUTPUT_ALLDATA "$PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0*28"
#define PMTK_SET_NMEA_OUTPUT_OFF "$PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"

class GPS{
  public:
    GPS(USARTClass *ser);
    GPS(UARTClass *ser);
    void begin(long baud);
    void println(char *str); //Prints to the serial port.
    char readChar(); //Reads one char at a time for debugging.
    int buildCompleteNMEAString(int timeout); //Waits for the begining of a NMEA string and then gatheres the entire string.
    char verifiedNMEAString[MAXSIZE]; //Holds last complete NMEA sentence generated, safe for parsing.
    
  private:  
    void init(); //Sets up default values.
    uint8_t parseHex(char c);
    boolean checksum(); //checks incomingNMEAStrings checksum.
    USARTClass *_serialPort; //Serial1 - Serial3 are USARTClass objects.
    UARTClass *_usbPort;  //Serial is a UARTClass object and output is mirrored to the usb.
    char incomingNMEAString[MAXSIZE]; //Complete NMEA strings built here. Do not use for parsing.
};

#endif