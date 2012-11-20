#include <Adafruit_BMP085.h>

// Ladyada's logger modified by Bill Greiman to use the SdFat library

// this is a generic logger that does checksum testing so the data written should be always good
// Assumes a sirf III chipset logger attached to pin 2 and 3
#include <Wire.h>
#include <SD.h>
#include <avr/sleep.h>
#include "GPSconfig.h"

// If using Arduino IDE prior to 1.0,
// make sure to install newsoftserial from Mikal Hart
// http://arduiniana.org/libraries/NewSoftSerial/
#if ARDUINO >= 100
 #include <SoftwareSerial.h>
#else
 #include <NewSoftSerial.h>
#endif

// power saving modes
#define SLEEPDELAY 0    /* power-down time in seconds. Max 65535. Ignored if TURNOFFGPS == 0 */
#define TURNOFFGPS 0    /* set to 1 to enable powerdown of arduino and GPS. Ignored if SLEEPDELAY == 0 */
#define LOG_RMC_FIXONLY 0  /* set to 1 to only log to SD when GPD has a fix */

// what to log
#define LOG_RMC 0 // RMC-Recommended Minimum Specific GNSS Data, message 103,04
#define LOG_GGA 1 // GGA-Global Positioning System Fixed Data, message 103,00
#define LOG_GLL 0 // GLL-Geographic Position-Latitude/Longitude, message 103,01
#define LOG_GSA 0 // GSA-GNSS DOP and Active Satellites, message 103,02
#define LOG_GSV 0 // GSV-GNSS Satellites in View, message 103,03
#define LOG_VTG 0 // VTG-Course Over Ground and Ground Speed, message 103,05



// Use pins 2 and 3 to talk to the GPS. 2 is the TX pin, 3 is the RX pin
// Use pins 6 and 7 to talk to the xbee 6 is the TX pin, 7 is the RX pin
#if ARDUINO >= 100
 SoftwareSerial gpsSerial =  SoftwareSerial(2, 3);
 SoftwareSerial xbee = SoftwareSerial(6,7);
#else
 NewSoftSerial gpsSerial =  NewSoftSerial(2, 3);
 NewSoftSerial xbee = NewSoftSerial(6,7);
#endif
// Set the GPSRATE to the baud rate of the GPS module. Most are 4800
// but some are 38400 or other. Check the datasheet!
#define GPSRATE 4800
#define XBEERATE 9600

// Set the pins used 
#define powerPin 4
#define led1Pin 5
#define led2Pin 6
#define chipSelect 10
#define BUFFSIZE 90



char buffer[BUFFSIZE]; //Used to store various strings troughout the program; file names, gps data, parsing data, etc
uint8_t bufferidx = 0;
bool fix = false; // current fix data
bool gotGPRMC;    //true if current data is a GPRMC strinng
uint8_t i;

Adafruit_BMP085 bmp; //Object used to communicate with the barometer using the Adafruit Barometer library
File logfile; //Object used to access the SD card to write data to it.
File dumpfile; //Object used to access the SD card and read from it to either the serial port or the xbee.

const int BarometerToGPSRatio = 2; //Controls how often to poll the barometer for each gps reading
const int AccelerometerToBarometerRatio = 20; //Controls how often to poll the accelerometers per barometer reading
const unsigned long dataTimeout = 900000; //Time to record data in milliseconds from when program starts, 900 000 = 15 minutes

boolean stringComplete = false; //toggles datadump
boolean gpscheckdisable = true; //logs data regardless of good gps data



/**************************************************************************
read a Hex value and return the decimal equivalent
*************************************************************************/
uint8_t parseHex(char c) {
  if (c < '0')
    return 0;
  if (c <= '9')
    return c - '0';
  if (c < 'A')
    return 0;
  if (c <= 'F')
    return (c - 'A')+10;
}


/*****************************************************************************
blink out an error code
*****************************************************************************/
void error(uint8_t errno) {
/*
  if (SD.errorCode()) {
    putstring("SD error: ");
    Serial.print(card.errorCode(), HEX);
    Serial.print(',');
    Serial.println(card.errorData(), HEX);
  }
  */
  while(1) {
    for (i=0; i<errno; i++) {
      digitalWrite(led1Pin, HIGH);
      digitalWrite(led2Pin, HIGH);
      delay(100);
      digitalWrite(led1Pin, LOW);
      digitalWrite(led2Pin, LOW);
      delay(100);
    }
    for (; i<10; i++) {
      delay(200);
    }
  }
}

void setup() {
  WDTCSR |= (1 << WDCE) | (1 << WDE);
  WDTCSR = 0;
  
  bmp.begin(); //Initializes barometer object used to read from barometer
  
  
  Serial.begin(9600);
  Serial.println("\r\nGPSlogger");
  
  xbee.begin(XBEERATE);
  xbee.println("\r\nGPSlogger");
  
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
  pinMode(powerPin, OUTPUT);
  digitalWrite(powerPin, LOW);

  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card init. failed!");
    error(1);
  }



// Finds the last existing logfile and creates a new one after it
// Comment out to use the same logfile.

  strcpy(buffer, "GPSLOG00.TXT");
 
  for (i = 0; i < 100; i++) {
    buffer[6] = '0' + i/10;
    buffer[7] = '0' + i%10;
    // create if does not exist, do not open existing, write, sync after write
    if (! SD.exists(buffer)) {
      break;
    }
  }

//Removes duplicate logfile, only matter if rewriting over the same file
//  if(SD.exists(buffer)){
//    SD.remove(buffer);
//    Serial.println("Deleting duplicate logfile");
//  }


  
  logfile = SD.open(buffer, FILE_WRITE);
  if( ! logfile ) {
    Serial.print("Couldnt create "); Serial.println(buffer);
    error(3);
  }
  Serial.print("Writing to "); Serial.println(buffer);



  
  // connect to the GPS at the desired rate
  gpsSerial.begin(GPSRATE);
  
  Serial.println("Ready!");
  
  gpsSerial.print(SERIAL_SET);
  delay(250);


//Sends the necessary commands to toggle various NMEA sentences on and off
//based on the defines set at the beginning of the program.
#if (LOG_DDM == 1)
     gpsSerial.print(DDM_ON);
#else
     gpsSerial.print(DDM_OFF);
#endif
  delay(250);//find
#if (LOG_GGA == 1)
    gpsSerial.print(GGA_ON);
#else
    gpsSerial.print(GGA_OFF);
#endif
  delay(250);//find
#if (LOG_GLL == 1)
    gpsSerial.print(GLL_ON);
#else
    gpsSerial.print(GLL_OFF);
#endif
  delay(250);//find
#if (LOG_GSA == 1)
    gpsSerial.print(GSA_ON);
#else
    gpsSerial.print(GSA_OFF);
#endif
  delay(250);//find
#if (LOG_GSV == 1)
    gpsSerial.print(GSV_ON);
#else
    gpsSerial.print(GSV_OFF);
#endif
  delay(250);//find
#if (LOG_RMC == 1)
    gpsSerial.print(RMC_ON);
#else
    gpsSerial.print(RMC_OFF);
#endif
  delay(250);//find

#if (LOG_VTG == 1)
    gpsSerial.print(VTG_ON);
#else
    gpsSerial.print(VTG_OFF);
#endif
  delay(250);//find

#if (USE_WAAS == 1)
    gpsSerial.print(WAAS_ON);
#else
    gpsSerial.print(WAAS_OFF);
#endif
}



/*********************************************************************************
Main Loop
*********************************************************************************/
void loop() {
  
  
  //Data aquisition loop
  while(millis() < dataTimeout){
    
    
    //Serial.println(Serial.available(), DEC);
    char c;
    uint8_t sum;
  
    // read one 'line'
    if (gpsSerial.available()) {
      c = gpsSerial.read();
      
      
  #if ARDUINO >= 100
      //Serial.write(c);
  #else
      //Serial.print(c, BYTE);
  #endif
      if (bufferidx == 0) {
        while (c != '$')
          c = gpsSerial.read(); // wait till we get a $
      }
      buffer[bufferidx] = c;
  
  #if ARDUINO >= 100
      //Serial.write(c);
  #else
      //Serial.print(c, BYTE);
  #endif
      if (c == '\n') {
        //putstring_nl("EOL");
        //Serial.print(buffer);
        buffer[bufferidx+1] = 0; // terminate it
  
        if (buffer[bufferidx-4] != '*') {
          // no checksum?
          Serial.print('*');
          bufferidx = 0;
          return;
        }
        // get checksum
        sum = parseHex(buffer[bufferidx-3]) * 16;
        sum += parseHex(buffer[bufferidx-2]);
  
        // check checksum
        for (i=1; i < (bufferidx-4); i++) {
          sum ^= buffer[i];
        }
        if (sum != 0) {
          //putstring_nl("Cxsum mismatch");
          Serial.print('~');
          bufferidx = 0;
          return;
        }
        // got good data!
  
        gotGPRMC = strstr(buffer, "GPRMC");
        if (gotGPRMC) {
          // find out if we got a fix
          char *p = buffer;
          p = strchr(p, ',')+1;
          p = strchr(p, ',')+1;       // skip to 3rd item
          
          if (p[0] == 'V') {
            digitalWrite(led1Pin, LOW);
            fix = false;
          } else {
            digitalWrite(led1Pin, HIGH);
            fix = true;
          }
        }
        if (LOG_RMC_FIXONLY) {
          if (!fix) {
            Serial.print('_');
            bufferidx = 0;
            return;
          }
        }
        // rad. lets log it!
        
        Serial.print(buffer);    //first, write it to the serial monitor
        Serial.print('#');
        
        if (gotGPRMC || gpscheckdisable)      //If we have a GPRMC string
        {
          // Bill Greiman - need to write bufferidx + 1 bytes to getCR/LF
          bufferidx++;
  
          digitalWrite(led2Pin, HIGH);      // Turn on LED 2 (indicates write to SD)
          
          
          logfile.print("time: ");
          logfile.println(millis());
          Serial.println(millis());
          logfile.write((uint8_t *) buffer, bufferidx);    //write the string to the SD file
          
//        Records data from other sensors while waiting for the gps.
          for(int i = 0; i < BarometerToGPSRatio; i++){

            //Every reading is timestamped
            logfile.print("time: ");
            logfile.println(millis());
            Serial.println(millis());
            
            int pressure = bmp.readPressure();
            int temperature = bmp.readTemperature();
            int altitude = bmp.readAltitude();
                        
            
            logfile.print("pressure: ");
            logfile.println(pressure);
            //Serial.println("pressure: " + String(pressure));
            
            logfile.print("temp: ");
            logfile.println(temperature);
            //Serial.println("temperature: " + String(temperature));
            
            logfile.print("altitude: ");
            logfile.println();
            //Serial.println("altitude: " + String(altitude));
            
            logfile.flush(); //Makes sure everything in the buffer is written to the file
            
            
            for(int j = 0; j < AccelerometerToBarometerRatio; j++){
              logfile.print("time: ");
              logfile.println(millis());
              Serial.println(millis());
              
              int xa = analogRead(A0);
              int xb = analogRead(1);
              int yb = analogRead(A2);
              int zb = analogRead(A3);

              logfile.print("x: ");
              logfile.println(xa);
              //Serial.println("xa: " + String(xa));
           
             logfile.print("x: ");
             logfile.println(xb);
             //Serial.println("xb: " + String(xb));
           
             logfile.print("y: ");
             logfile.println(yb);
             //Serial.println("yb: " + String(yb));
          
             logfile.print("z: ");
             logfile.println(zb);
             //Serial.println("zb: " + String(zb));
             logfile.flush();
            }
          }
          logfile.flush();
          
          /*
          if( != bufferidx) {
             putstring_nl("can't write!");
             error(4);
          }
          */
  
          digitalWrite(led2Pin, LOW);    //turn off LED2 (write to SD is finished)
  
          bufferidx = 0;    //reset buffer pointer
  
          if (fix) {  //(don't sleep if there's no fix)
            
            if ((TURNOFFGPS) && (SLEEPDELAY)) {      // turn off GPS module? 
            
              digitalWrite(powerPin, HIGH);  //turn off GPS
  
              delay(100);  //wait for serial monitor write to finish
              sleep_sec(SLEEPDELAY);  //turn off CPU
    
              digitalWrite(powerPin, LOW);  //turn on GPS
            } //if (TURNOFFGPS) 
           
          } //if (fix)
          
          return;
        }//if (gotGPRMC)
        
      }
      bufferidx++;
      if (bufferidx == BUFFSIZE-1) {
         Serial.print('!');
         bufferidx = 0;
      }
    } else {
  
    }
  }
  logfile.close();
  
  
  /************************************************
  Waits for the user to send an 'a' and then dumps the latest datalog through the serial port
  ************************************************/
  
  Serial.println("Done recording, press 'a' to retrieve data");
  delay(250);
  
  if(stringComplete){
    stringComplete = false;
    strcpy(buffer, "GPSLOG00.TXT");
   
    //finds the last log file and opens it for reading 
    for (i = 0; i < 100; i++) {
      buffer[6] = '0' + i/10;
      buffer[7] = '0' + i%10;
      if (! SD.exists(buffer)) {
        break;
      }
    }
    
    if(buffer[7] == '0'){
      buffer[6] = buffer[6] - 1;
      buffer[7] = '9';
    }
    else{
      buffer[7] = buffer[7] - 1;
    }
    
    
    Serial.print("Reading from "); Serial.println(buffer);
    dumpfile = SD.open(buffer, FILE_READ);
    

    if(dumpfile){
      while(dumpfile.available()){
        Serial.write(dumpfile.read());
      }
    }
    else {
      Serial.println("error opening logfile");
    } 
  }

}


/************************************************************
Watches serial port for a specific command.
When it recieves the command it sets string complete to true
************************************************************/
void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read(); 
    if (inChar == 'a') {
      stringComplete = true;
    } 
  }
}


void sleep_sec(uint16_t x) {
  while (x--) {
     // set the WDT to wake us up!
    WDTCSR |= (1 << WDCE) | (1 << WDE); // enable watchdog & enable changing it
    WDTCSR = (1<< WDE) | (1 <<WDP2) | (1 << WDP1);
    WDTCSR |= (1<< WDIE);
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
 //   sleep_enable();
    sleep_mode();
//    sleep_disable();
  }
}

SIGNAL(WDT_vect) {
  WDTCSR |= (1 << WDCE) | (1 << WDE);
  WDTCSR = 0;
}

/* End code */
