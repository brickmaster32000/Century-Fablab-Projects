/***********************Header Section***********************************************/
#define MAXSIZE 90 


class GPS{
  public:
    GPS(USARTClass *ser);
    GPS(UARTClass *ser);
    void begin(long baud);
    void println(char *str); //Prints to the serial port.
    char readChar(); //Reads one char at a time for debugging.
    int buildCompleteNMEAString(int timeout); //Waits for the begining of a NMEA string and then gatheres the entire string
    char incomingNMEAString[MAXSIZE];
  private:
     
    void init(); //Sets up default values
    USARTClass *_serialPort; //Serial1 - Serial3 are USARTClass objects.
    UARTClass *_usbPort;  //Serial is a UARTClass object and output is mirrored to the usb
};
/*******************Test Program*********************************************************/



GPS bar(&Serial1);
char c = 0;

void setup(){
  Serial.begin(115200);
  
  bar.begin(9600);
  bar.println("$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29");
  bar.println("$PMTK220,200*2C");
}

void loop(){
  if(bar.buildCompleteNMEAString(3000)){
    Serial.print(bar.incomingNMEAString);
  }
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
  
  while(millis() < (startTime + timeout) && i < (MAXSIZE - 2)){
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
      return 1;
    }
  }
  return 0; //Timed out
}
    
      
    
  
