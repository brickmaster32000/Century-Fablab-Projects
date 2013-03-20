/******************************function declarations ******************************/
#include <GPS.h>

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