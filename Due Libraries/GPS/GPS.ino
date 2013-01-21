/***********************Header Section***********************************************/
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
/*******************Test Program*********************************************************/



GPS bar(&Serial1);
char c = 0;

void setup(){
  Serial.begin(115200);
  
  bar.begin(9600);
  bar.println(PMTK_SET_NMEA_OUTPUT_GGAONLY);
  bar.println(PMTK_SET_NMEA_UPDATE_5HZ);
}

void loop(){
  while(!bar.buildCompleteNMEAString(3000))
    ;
  Serial.print(bar.verifiedNMEAString);
  //while(!bar.buildCompleteNMEAString(3000))
  //  ;
  //Serial.print(bar.verifiedNMEAString);
  delay(100);
}


/******************************function declerations ******************************/


GPS::GPS(USARTClass *ser){
  init();
  _serialPort = ser;
}

GPS::GPS(UARTClass *ser){
  init();
  _usbPort = ser;
}

void GPS::init(){
  
  //Need to set to null to check which pointer is in use.
  _serialPort = NULL; 
  _usbPort = NULL;
}

void GPS::begin(long baud){
  if(_serialPort){
    _serialPort->begin(baud);
  }else{
    _usbPort->begin(baud);
  }
}

//Mirrors the println() function on whichever serial port is used.
void GPS::println(char *str){
  if(_serialPort){
    _serialPort->println(str);
  }else{
    _usbPort->println(str);
  }
}

char GPS::readChar(){
  if(_serialPort){ 
    if(_serialPort->available()){
      return _serialPort->read();
    }else{ //If nothing is in the buffer return 0.
      return 0; 
    }
  }else{
    if(_usbPort->available()){
      return _usbPort->read();
    }else{ //If nothing is in the buffer return 0.
      return 0;
    }
  }
}

int GPS::buildCompleteNMEAString(int timeout){
  unsigned long startTime = millis();
  int i = 0;
  char c = 0;
  
  while(millis() < (startTime + timeout) && i < (MAXSIZE - 1)){
    if(_serialPort){
      while(!_serialPort->available())
        ; //Wait for data
      c = _serialPort->read();
    }else{
      while(!_usbPort->available())
        ; //Wait for data
      c = _usbPort->read();
    }
    
    if(i == 0 && c == '$'){
      incomingNMEAString[i++] = c; 
    }else if(i != 0){ //Only add to string after we grap the '$'
     incomingNMEAString[i++] = c;      
    }
    
    if(c == '\n' && i != 0){ //We have hit the end of the NMEA string; null terminate it and return.
      incomingNMEAString[i] = '\0';
      if(checksum()){
        for(int i = 0; i < MAXSIZE; i++){
          verifiedNMEAString[i] = incomingNMEAString[i];
          if(incomingNMEAString[i] == '\0')
            return 1;
        }
      }
      return 0;
    }
  }
  return 0; //Timed out
}


uint8_t GPS::parseHex(char c){
    if (c < '0')
      return 0;
    if (c <= '9')
      return c - '0';
    if (c < 'A')
       return 0;
    if (c <= 'F')
       return (c - 'A')+10;
}
    
boolean GPS::checksum(){
  uint8_t sum = 0;
  uint8_t runningSum = 0;
  int i = 1;
  
  while(incomingNMEAString[i] != '*' && i < (MAXSIZE - 3)){
    runningSum ^= incomingNMEAString[i++];
  }
  
  i++; //advance past '*'
  sum = (parseHex(incomingNMEAString[i++]) * 16);
  sum += parseHex(incomingNMEAString[i]);
  
  if(sum == runningSum)
    return true;
  else
    return false;
}
  
  
  
    
  
