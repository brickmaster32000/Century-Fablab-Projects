#include <Wire.h>
#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;


  
void setup() {
  Serial.begin(9600);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }
}
  
void loop() {
double x, maxVal = 1, minVal = 300;
long avg;
  
  Serial.println(millis());
  for(int i=0; i<1000; i++){
   x = bmp.readAltitude();
   if(x > maxVal){
     maxVal = x;}
     else if(x < minVal){
       minVal = x;}
   avg += x;
  }
  Serial.print("max=");
  Serial.print(maxVal);
  Serial.print("   min=");
  Serial.print(minVal);
  Serial.print("   avg=");
  Serial.println(avg/1000);
  Serial.println(millis());
  Serial.println(); 
}

