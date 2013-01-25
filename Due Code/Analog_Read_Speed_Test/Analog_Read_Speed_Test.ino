#include <Adafruit_BMP085.h>
#include <Wire.h>

Adafruit_BMP085 bmp;

int xA = A2;
int yA = A3;
int zA = A4;

int xB = A5;
int yB = A6;
int zB = A7;

void setup(){
  Serial.begin(115200);  
}

void loop(){
  
 
    Serial.print(millis());
    Serial.print(",xA,");Serial.print(analogRead(xA));
    Serial.print(",yA,");Serial.print(analogRead(yA));
    Serial.print(",zA,");Serial.print(analogRead(zA));
    Serial.print(millis());Serial.print(",");
    Serial.print(",xB,");Serial.print(analogRead(xB));
    Serial.print(",yB,");Serial.print(analogRead(yB));
    Serial.print(",zB,");Serial.println(analogRead(zB));
  
}
