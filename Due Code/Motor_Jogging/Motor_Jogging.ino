int const ledPin = 7;
int const motorControl = 6;
int const ledButton = 5;
int const motorButton = 4;

boolean armed = false;

void setup(){
  pinMode(ledPin, OUTPUT);
  pinMode(motorControl, OUTPUT);
  
  pinMode(ledButton, INPUT);
  pinMode(motorButton, INPUT);
  
  digitalWrite(ledPin, LOW);
  digitalWrite(motorControl, LOW);
}

void loop(){
  while(!digitalRead(ledButton) && !armed){
    if(digitalRead(motorButton)){
      analogWrite(motorControl, 255); //Need to start motor at full power to start spinning
      delay(10);
      analogWrite(motorControl, 50);
      delay(100);
    }else{
      analogWrite(motorControl, 0);
    }
  }
  armed = true;
  digitalWrite(ledPin, HIGH);
  
  delay(100);
    
}
