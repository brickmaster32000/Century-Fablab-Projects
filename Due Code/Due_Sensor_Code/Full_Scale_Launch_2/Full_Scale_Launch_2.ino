#include <GPS.h>
#include <Adafruit_BMP085.h>
#include <Wire.h>
#include <math.h>

void accelerometerRead(int offset);
void barometerRead(int offset);
void GPSRead(int offset);

double findApogee(double v0, double y0);

double vt; //terminal velocity
double g; //gravitiational acceleration
double targetHeight;
GPS gps(&Serial1);
Adafruit_BMP085 bmp;

// the ratios are not final, they need to be tuned to a realistic value
const int accelToBaromRatio = 10; // maybe half a second based on data received. 
boolean armed = false;

int xA = A2;
int yA = A3;
int zA = A4;

int xB = A5;
int yB = A6;
int zB = A7;

int proximity = A8;

int motorControl = 8;
int motorButton = 11;

void setup(){
  Serial.begin(9600);
  gps.begin(9600);
  bmp.begin();
 
  analogReadResolution(12);
  
  pinMode(motorButton, INPUT);
  
  //When the arduino is truned on holding the the motor button
  //for more than 2 seconds arms it
  if(digitalRead(motorButton)){
    int startTime = millis();
    while(digitalRead(motorButton)){}
    int endTime = millis();
    if(endTime - startTime > 2000){
      armed = true;
    }
  }
  
  //Locks into motor control mode if arduino wasnt armed.
  while(!armed){
    if(digitalRead(motorButton)){
      analogWrite(motorControl, 255);
      delay(50);
      analogWrite(motorControl, 125);
      delay(200);
    }else{
      analogWrite(motorControl, 0);
    }  
  } 
  
                //  1  ,2,  3,4,  5,6,  7,8,     9   ,10,    11    ,12,   13  ,14,   15,  ,16 
  Serial.println("TimeB,  ,xB,  ,yB,  ,zB, ,TimeBrake,  ,BrakePosit,  ,TimeBar,  ,Altitude,  ");
}

void loop(){
  //Gets two seperate
  float altitude = 0;
  long time[4]; //holds beginning and end time of altitude samples
  
  time[0] = millis();
  for(int i = 0; i < 5; i++){
    altitude += bmp.readAltitude();
  }
  float altitudeStart = altitude / 5;
  time[1] = millis();
  
  delay(500);
  altitude = 0;
  
  time[2] = millis();
  for(int i = 0; i < 5; i++){
    altitude += bmp.readAltitude();
  }
  float altitudeEnd = altitude / 5;
  time[3] = millis();
  
  double velocity = (altitudeEnd - altitudeStart)/(((time[3]-time[2])/2)-((time[1]-time[0])/2));
  double apogee = findApogee(velocity, altitudeEnd);
  
  if(apogee >= targetHeight){
    analogWrite(motorControl, 255);
    delay(1000);
    analogWrite(motorControl, 0);
  }
  
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
  
double findApogee(double v0, double y0){
  return y0 + (pow(vt, 2)/(2 * g))* log10((pow(v0, 2)+pow(vt, 2))/pow(vt,2));
}
  
