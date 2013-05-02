#include <Adafruit_GPS.h>
#if ARDUINO >= 100
 #include <SoftwareSerial.h>
#else
  // Older Arduino IDE requires NewSoftSerial, download from:
  // http://arduiniana.org/libraries/newsoftserial/
// #include <NewSoftSerial.h>
 // DO NOT install NewSoftSerial if using Arduino 1.0 or later!
#endif
#include <Wire.h>
#include <LSM303.h>


#if ARDUINO >= 100
  SoftwareSerial mySerial(2, 3);
#else
  NewSoftSerial mySerial(2, 3);
#endif


#define GPSECHO  true

void driveStraight();
double testDrive(long driveTime);
void orientDrive();
void driveStop();
void turnRadians(double rads);



// this keeps track of whether we're using the interrupt
// off by default!
boolean usingInterrupt = false;
void useInterrupt(boolean);

LSM303 compass;

Adafruit_GPS gps(&mySerial);
char c = 0;

double lastLatitude = 0;
double lastLongitude = 0;

double currentLatitude = 0;
double currentLongitude = 0;

double destinationLatitude = 32541;
double destinationLongitude = 4523;

double currentAngle = 0;
double destinationAngle = 0;

boolean activated = false;
boolean atDestination = false;

long checkDelay = 20000; //time between orientation checks after initial test drive in milliseconds
const double TOLERANCE = .0011; //set to 2m which is estimated GPS error

//set drive signals to pins
int enableL = 5;
int enableR = 6;
int driveR1 = 8;
int driveR2 = 9;
int driveL1 = 10;
int driveL2 = 11;


// Interrupt is called once a millisecond, looks for any new GPS data, and stores it
SIGNAL(TIMER0_COMPA_vect) {
  char c = gps.read();
  // if you want to debug, this is a good time to do it!
  if (GPSECHO)
    if (c) UDR0 = c;  
    // writing direct to UDR0 is much much faster than Serial.print 
    // but only one character can be written at a time. 
}

void useInterrupt(boolean v) {
  if (v) {
    // Timer0 is already used for millis() - we'll just interrupt somewhere
    // in the middle and call the "Compare A" function above
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
    usingInterrupt = true;
  } else {
    // do not call the interrupt function COMPA anymore
    TIMSK0 &= ~_BV(OCIE0A);
    usingInterrupt = false;
  }
}

void setup(){
  //change driveR and driveL to ouput pins
  pinMode(driveR1, OUTPUT); 
  pinMode(driveL1, OUTPUT);
  pinMode(driveR2, OUTPUT); 
  pinMode(driveL2, OUTPUT);
  pinMode(enableR, OUTPUT);
  pinMode(enableL, OUTPUT);

  //prep xBee
  Serial.begin(9600);

  //prep GPS  
  gps.begin(9600);
  gps.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  gps.sendCommand(PMTK_SET_NMEA_UPDATE_5HZ);
  useInterrupt(true);


  //prep Magnetometer
  Wire.begin();
  compass.init();
  compass.enableDefault();
  //put in good calibration values! these are from FabLab 12APR2013!
  compass.m_min.x = -697; compass.m_min.y = -622; compass.m_min.z = -538;
  compass.m_max.x = +472; compass.m_max.y = +502; compass.m_max.z = 889;
}

uint32_t timer = millis();
void loop(){
  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis())  timer = millis();
  
  while(!gps.newNMEAreceived()) 
        ; //While no new gps recieved keep looping
  gps.parse(gps.lastNMEA());  
  currentLatitude = gps.latitude;
  currentLongitude = gps.longitude;
  Serial.print("Latitude,");Serial.print(currentLatitude);
  Serial.print(",Longitude,");Serial.println(currentLongitude);
  
  while(activated){
     
    while(!gps.newNMEAreceived()) 
        ; //While no new gps recieved keep looping
      gps.parse(gps.lastNMEA());  
      currentLatitude = gps.latitude;
      currentLongitude = gps.longitude;
      
    currentAngle = testDrive(10000);
    while(!gps.newNMEAreceived()) 
        ; //While no new gps recieved keep looping
    gps.parse(gps.lastNMEA());  
    currentLatitude = gps.latitude;
    currentLongitude = gps.longitude;
    destinationAngle = atan2((destinationLatitude - currentLatitude), (destinationLongitude - currentLongitude));
    Serial.print("Latitude,");Serial.print(currentLatitude);
    Serial.print(",Longitude,");Serial.print(currentLongitude);
    Serial.print(",Destination Angle,");Serial.print(destinationAngle);
    turnRadians((destinationAngle - currentAngle)+ PI); // psuedo code to turn rover so many radians.
    
    lastLatitude = currentLatitude;
    lastLongitude = currentLongitude;
    
      
    /************************************************************************************
    Drive for a while, then check the Rover's current orientation against
    the destination angle and corrects angle if necessary
    ************************************************************************************/  
    while(!atDestination){
      orientDrive();
      driveStraight(); //code to drive rover straight
      
      long time = millis();
      while(millis() - time < checkDelay)
        ; // drive straight until outside of gps error range
      
      driveStop(); //code to stop rover
      while(!gps.newNMEAreceived()) 
        ; //While no new gps recieved keep looping
      gps.parse(gps.lastNMEA());  
      currentLatitude = gps.latitude;
      currentLongitude = gps.longitude;
      
      currentAngle = atan2((currentLatitude - lastLatitude), (currentLongitude - lastLongitude)); 
      destinationAngle = atan2((destinationLatitude - currentLatitude), (destinationLongitude - currentLongitude));
      Serial.print("Latitude,");Serial.print(currentLatitude);
      Serial.print(",Longitude,");Serial.print(currentLongitude);
      Serial.print(",Destination Angle,");Serial.print(destinationAngle);
      turnRadians(destinationAngle - currentAngle); // psuedo code to turn rover so many radians.
      
      lastLatitude = currentLatitude;
      lastLongitude = currentLongitude;
      
      if((currentLatitude - destinationLatitude) < TOLERANCE && (currentLongitude - destinationLongitude) < TOLERANCE){
        atDestination = true;
      }
    
    }
  }
}



/*************************************************************************************************
Drives in current direction for the time set by drive time
At the end of the drive it compares its current gps location with
its start position and determines the angle it must be facing either
North or South of East.

Angle is in radians and between +pi and -pi
************************************************************************************************/
double testDrive(long driveTime){
  while(!gps.newNMEAreceived()) 
    ; //While no new gps recieved keep looping
  gps.parse(gps.lastNMEA());  
  lastLatitude = gps.latitude;
  lastLongitude = gps.longitude;
  long time = millis();
  
  orientDrive();
  driveStraight(); //psuedocode to drive rover straight 
  while(millis() - time < driveTime)
    ; // drive straight until outside of gps error range
  driveStop(); //psuedocode to stop rover
  
  while(!gps.newNMEAreceived()) 
    ; //While no new gps recieved keep looping
  gps.parse(gps.lastNMEA());
  currentLatitude = gps.latitude;
  currentLatitude = gps.longitude;
  
  return atan2((currentLatitude - lastLatitude), (currentLongitude - lastLongitude));  
}

void orientDrive(){
  compass.read();
  if((int)compass.a.z > 1){
    digitalWrite(driveR1, HIGH);
    digitalWrite(driveL1, HIGH);
    digitalWrite(driveR2, LOW);
    digitalWrite(driveL2, LOW);
  }
  else if((int)compass.a.z < 0){
    digitalWrite(driveR2, HIGH);
    digitalWrite(driveL2, HIGH);
    digitalWrite(driveR1, LOW);
    digitalWrite(driveL1, LOW); 
  }
}

void driveStraight(){ 
  digitalWrite(enableR, HIGH);
  digitalWrite(enableL, HIGH);
}

void driveStop(){
  digitalWrite(enableR, LOW);
  digitalWrite(enableL, LOW);
}

void turnRadians(double rads){
  double radsTurned = 0;
  
  compass.read();
  int bearing1 = compass.heading((LSM303::vector){0,-1,0});
  
  while(radsTurned < rads){
    orientDrive();
    analogWrite(enableR, 125);
    compass.read();
    int bearing2 = compass.heading((LSM303::vector){0,-1,0});
    double radsTurned = ((bearing2 - bearing1) * (PI/180));
  }
  digitalWrite(enableR, LOW);
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read(); 

    // if the incoming character is a g, set a flag
    // so the main loop can do something about it:
    if (inChar == 'g') {
      activated = true;
    } 
  }
}
