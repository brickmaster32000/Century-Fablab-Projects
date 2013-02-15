

void setup() {
  analogReadResolution(12);
  Serial.begin(9600);
  
  
  delay(3000);
  Serial.println("Begin");
  delay(1000);
  for(int i = 0; i < 500; i++){
    Serial.print("x1,");Serial.print(analogRead(A2));
    Serial.print(",y1,");Serial.print(analogRead(A3));
    Serial.print(",z1,");Serial.print(analogRead(A4));
    Serial.print(",x2,");Serial.print(analogRead(A5));
    Serial.print(",y2,");Serial.print(analogRead(A6));
    Serial.print(",z2,");Serial.print(analogRead(A7));Serial.print('\n');
    
  }
  

}

void loop() {
  
  
}
