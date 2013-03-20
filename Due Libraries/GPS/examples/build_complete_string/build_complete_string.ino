#include<GPS.h>

GPS bar(&Serial1);
char c = 0;

void setup(){
  Serial.begin(115200);
  
  bar.begin(9600);
  bar.println(PMTK_SET_NMEA_OUTPUT_GGAONLY);
  bar.println(PMTK_SET_NMEA_UPDATE_5HZ);
}

void loop(){
  while(!bar.buildCompleteNMEAString(3000))
    ;
  Serial.print(bar.verifiedNMEAString);
  //while(!bar.buildCompleteNMEAString(3000))
  //  ;
  //Serial.print(bar.verifiedNMEAString);
  delay(100);
}