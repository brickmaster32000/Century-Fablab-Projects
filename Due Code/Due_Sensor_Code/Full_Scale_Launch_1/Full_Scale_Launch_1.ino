#include <GPS.h>
#include <Adafruit_BMP085.h>
#include <Wire.h>

void accelerometerRead(int offset);
void barometerRead(int offset);
void GPSRead(int offset);

GPS gps(&Serial1);
Adafruit_BMP085 bmp;

// the ratios are not final, they need to be tuned to a realistic value
const int accelToBaromRatio = 10; // maybe half a second based on data received. 

int xA = A2;
int yA = A3;
int zA = A4;

int xB = A5;
int yB = A6;
int zB = A7;

int proximity = A8;

void setup(){
  Serial.begin(9600);
  gps.begin(9600);
  bmp.begin();
 
  analogReadResolution(12);
  
  delay(500);
  
                //  1  ,2,  3,4,  5,6,  7,8,     9   ,10,    11    ,12,   13  ,14,   15,  ,16 
  Serial.println("TimeB,  ,xB,  ,yB,  ,zB, ,TimeBrake,  ,BrakePosit,  ,TimeBar,  ,Altitude,  ");
}

void loop(){
  for(int i = 0; i < accelToBaromRatio; i++){
    accelerometerRead(0);
  }
  barometerRead(12);
}

/******************************************************************************
Takes a set of accelerometer readings and prints them
offset is used to add commas at the begining of the 
string to position it in the proper collum
******************************************************************************/
void accelerometerRead(int offset){
  for(int i = 0; i < offset; i++){
    Serial.print(',');
  }
  
  Serial.print("TimeB,");Serial.print(millis());
  Serial.print(",xB,");Serial.print(analogRead(xB));
  Serial.print(",yB,");Serial.print(analogRead(yB));
  Serial.print(",zB,");Serial.print(analogRead(zB));
  Serial.print(",BrakePosit,");Serial.println(analogRead(proximity));
}

void barometerRead(int offset){
  for(int i = 0; i < offset; i++){
    Serial.print(',');
  }
  
  Serial.print("TimeBar,");Serial.print(millis());
  Serial.print(",Altitude,");Serial.println(bmp.readAltitude());
}
  
  
  
