
#include <GPS.h>
#include <Wire.h>

GPS gps(&Serial1);

// the ratios are not final, they need to be tuned to a realistic value
const int accelToGPS = 143;
const int ACTIVATION = 200;
 
int xA = A2;
int yA = A3;
int zA = A4;

int xB = A5;
int yB = A6;
int zB = A7;

int proximity = A8;
int brakeDist = proximity;

boolean brake = false;


void setup(){
  Serial.begin(9600);
  gps.begin(9600);
  
  analogReadResolution(12);
   
  gps.println(PMTK_SET_NMEA_OUTPUT_GGAONLY);
  gps.println(PMTK_SET_NMEA_UPDATE_5HZ);
  
  delay(500);
  
               //1,  2  ,3, 4,5, 6,7, 8,9, 10  ,11,12,13,14,15,16,    17    ,     18    ,    19    ,   20   ,    21   ,  
  Serial.println(",TimeA, ,xA, ,yA, ,zA, ,TimeB,  ,xB,  ,yB,  ,zB, TimeBrake, BrakePosit, Activated, TimeBar, Altitude, ,TimeGPS, Latitude,, Longitude,,Quality,SAT#,,AltitudeGPS,,,,"); // Include headers at the top of the file for csv ease
}
  
void loop(){
  
  
  while(!gps.buildCompleteNMEAString(3000))
  ;
  Serial.print(",,,,,,,,,,,,,,,,,,,,,");Serial.print(gps.verifiedNMEAString);
  while(!gps.buildCompleteNMEAString(3000))
  ;
  Serial.print(",,,,,,,,,,,,,,,,,,,,,");Serial.print(gps.verifiedNMEAString);
  
  //Print accelerometer data and drag brake when not printing GPS
  for(int j = 0; j < accelToGPS; j++){
    
    Serial.print("TimeA,");Serial.print(millis());
    Serial.print(",xA,");Serial.print(analogRead(xA));
    Serial.print(",yA,");Serial.print(analogRead(yA));
    Serial.print(",zA,");Serial.print(analogRead(zA));
    Serial.print(",TimeB,");Serial.print(millis());
    Serial.print(",xB,");Serial.print(analogRead(xB));
    Serial.print(",yB,");Serial.print(analogRead(yB));
    Serial.print(",zB,");Serial.print(analogRead(zB));
    
    brakeDist = analogRead(proximity);
    Serial.print(",");Serial.print(millis());
    Serial.print(",");Serial.println(brakeDist);
    
    if(brakeDist > ACTIVATION && brake !=true){
      brake=true;
      Serial.print(",,,,,,,,,,,,,,,,,");Serial.print(millis());
      Serial.print(",");Serial.print(brakeDist);
      Serial.print(",");Serial.println("Activated");
    }
  }
}
