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
  double v, alt1, alt2, dAlt, t1, t2, dT, groundAlt = 234;
  double altThresh = 1609.34 + groundAlt, altProj;
  boolean deployed = false;
  
  while(deployed == false){
    alt1 = bmp.readAltitude();
    t1 = millis();
    alt2 = bmp.readAltitude();
    t2 = millis();
    
    dAlt = alt2-alt1;
    dT = t2-t1;
    v = dAlt / dT;
    
    altProj = ();
    if(altProj >= altThresh){
      deploy;
      deployed = true;
    }
  
  }
}

