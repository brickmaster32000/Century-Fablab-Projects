

void setup() {
  analogReadResolution(12);
  Serial.begin(9600);
  
  for(int i = 0; i < 10; i++){
    Serial.print("x1,");Serial.print(analogRead(A2));
    Serial.print(",y1,");Serial.print(analogRead(A3));
    Serial.print(",z1,");Serial.print(analogRead(A4));
    Serial.print(",x2,");Serial.print(analogRead(A5));
    Serial.print(",y2,");Serial.print(analogRead(A6));
    Serial.print(",z2,");Serial.println(analogRead(A7));
  }
  

}

void loop() {
  
  
}
