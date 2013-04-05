#include <GPS.h>
 
GPS gps(&Serial);
char c = 0;

double lastLatitude = 0;
double lastLongitude = 0;

double currentLatitude = 0;
double currentLongitude = 0;

double destinationLatitude = 32541;
double destinationLongitude = 4523;

double currentAngle = 0;
double destinationAngle = 0;

boolean inAir = true;
boolean atDestination = false;

long checkDelay = 20000; //time between orientation checks after initial test drive in milliseconds
const double TOLERANCE = .0011; //set to 2m which is estimated GPS error

//set drive signals to pins
int driveR = 7;
int driveL = 6;

void setup(){
  //change driveR and driveL to ouput pins
  pinMode(driveR, OUTPUT); 
  pinMode(driveL, OUTPUT);
  
  gps.begin(9600);
  gps.println(PMTK_SET_NMEA_OUTPUT_GGAONLY);
  gps.println(PMTK_SET_NMEA_UPDATE_5HZ);

}

void loop(){
  
  while(inAir){
    
  };
  
  currentAngle = testDrive(10000);
  destinationAngle = atan2((destinationLatitude - currentLatitude), (destinationLongitude - currentLongitude));
  turnRadians(destinationAngle - currentAngle); // psuedo code to turn rover so many radians.
  
  lastLatitude = currentLatitude;
  lastLongitude = currentLongitude;
  
    
  /************************************************************************************
  Drive for a while, then check the Rover's current orientation against
  the destination angle and corrects angle if necessary
  ************************************************************************************/  
  while(!atDestination){
    driveStraight(); //code to drive rover straight
    
    time = millis();
    while(millis() - time < checkDelay)
      ; // drive straight until outside of gps error range
    
    driveStop(); //code to stop rover
    while(!gps.newNMEARecieved()) 
      ; //While no new gps recieved keep looping
    gps.parse(lastNMEA());  
    currentLatitude = gps.latitude;
    currentLongitude = gps.longtiude;
    
    currentAngle = atan2((currentLatitude - lastLatitude), (currentLongitude - lastLongitude)); 
    destinationAngle = atan2((destinationLatitude - currentLatitude), (destinationLongitude - currentLongitude));
    turnRadians(destinationAngle - currentAngle); // psuedo code to turn rover so many radians.
    
    lastLatitude = currentLatitude;
    lastLongitude = currentLongitude;
    
    if((currentLatitude - destinationLatitude) < TOLERANCE && (currentLongitude - destinationLongitude) < TOLERANCE){
      atDestination = true;
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
  while(!gps.newNMEARecieved()) 
    ; //While no new gps recieved keep looping
  gps.parse(lastNMEA());  
  lastLatitude = gps.latitude;
  lastLongitude = gps.longtiude;
  long time = millis();
  
  driveStraight(); //psuedocode to drive rover straight 
  while(millis() - time < driveTime)
    ; // drive straight until outside of gps error range
  driveStop(); //psuedocode to stop rover
  
  while(!gps.newNMEARecieved()) 
    ; //While no new gps recieved keep looping
  gps.parse(lastNMEA());
  currentLatitude = gps.latitude;
  currentLatitude = gps.longitude;
  
  return atan2((currentLatitude - lastLatitude), (currentLongitude - lastLongitude));  
}

void driveStraight(){
  digitalWrite(driveR, HIGH);
  digitalWrite(driveR, HIGH);
}

void driveStop(){
  digitalWrite(driveR, LOW);
  digitalWrite(driveR, LOW); 
}
