

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
  double latitudeOrigin = gps.latitude;
  double longitudeOrigin = gps.longtiude;
  long time = millis();
  
  driveStraight(); //psuedocode to drive rover straight 
  while(millis() - time < driveTime)
    ; // drive straight until outside of gps error range
  driveStop(); //psuedocode to stop rover
  
  while(!gps.newNMEARecieved()) 
    ; //While no new gps recieved keep looping
  gps.parse(lastNMEA());
  double latitudeNew = gps.latitude;
  double longitudeNew = gps.longitude;
  
  return atan2((latitudeNew - latitudeOrigin), (longitudeNew - longitudeOrigin));  
}
 

