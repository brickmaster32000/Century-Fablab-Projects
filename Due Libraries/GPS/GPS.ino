class GPS{
  public:
    GPS(USARTClass *ser);
    GPS(UARTClass *ser);
    void begin(long baud);
    void println(char *str);
  private:
    void init(); //Sets up default values
    USARTClass *_serialPort; //Serial1 - Serial3 are USARTClass objects, Serial is a UARTClass object
    UARTClass *_usbPort;
};

GPS foo(&Serial);
GPS bar(&Serial1);


void setup(){
  foo.begin(9600);
  bar.begin(9600);
}

void loop(){
  foo.println("Hello USB!");
  bar.println("Hello XBEE!");
  delay(500);
}

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
