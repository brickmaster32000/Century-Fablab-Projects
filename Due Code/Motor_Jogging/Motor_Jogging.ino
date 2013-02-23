int const AIRBRAKEDELAY = 2000;// TIme to wait after detecting burnout to deploy brake.
int const TRIGGER = 1520; //Value to stardrag brake deployment from. 

int const YA = A0;

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
  
  analogReadResolution(12);
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
  
  while(analogRead(YA) > TRIGGER){} //Idle until a negative acceleration is detected.
  
  delay(AIRBRAKEDELAY);
  
  analogWrite(motorControl, 255);
  delay(1000);
  analogWrite(motorControl, 0);
  
  while(true){} //Air brake has been deployed; idle till reset.
  
    
}
