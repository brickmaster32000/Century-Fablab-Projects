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
  Serial.println(millis());
  Serial.print("xA, ");Serial.println(analogRead(xA));
  
  Serial.println(millis());
  Serial.print("yA, ");Serial.println(analogRead(yA));
  
  Serial.println(millis());
  Serial.print("zA, ");Serial.println(analogRead(zA));
  
  Serial.println(millis());
  Serial.print("xB, ");Serial.println(analogRead(xB));
  
  Serial.println(millis());
  Serial.print("yB, ");Serial.println(analogRead(yB));
  
  Serial.println(millis());
  Serial.print("zB, ");Serial.println(analogRead(zB));
    
}
