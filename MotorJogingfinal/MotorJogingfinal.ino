const int armSwitch = 8; //input
const int motorSwitch = 9; //input
const int motorControl = 11; //output
const int accelerometer =  A1; 

//1g ~= 393
//0g ~= 325
//-1g ~= 257 
//

const int trigger = 305; 
const int airbrakeDelay = 2000; //delay after trigger to deploy airbrake in milliseconds


//flags to make sure arming and air brake deploying only happens once
boolean armed = false;  
boolean deployed = false;

void setup(){
  Serial.begin(9600);
  
  pinMode(armSwitch, INPUT);
  pinMode(motorSwitch, INPUT);
  pinMode(motorControl, OUTPUT);
}

void loop(){
  
//Allows user to jog motor until the armed switchis toggled.
//Prints accelerometer data for debug and calibration use.
  while((!armed) && digitalRead(armSwitch)){
    
    Serial.print("Accelerometer, ");
    Serial.print(accelerometer);
    Serial.print("; ");
    Serial.println(analogRead(accelerometer));
    
    //Jogs motor slowly when motor switch is toggled
    if(digitalRead(motorSwitch)){
      analogWrite(motorControl, 255);
      delay(10);
      analogWrite(motorControl, 20);
      delay(100);
    }
    else{
      analogWrite(motorControl, 0);
    }
  }
  
  
  //Turns off motor and prevents it from turning back on after being armed
  if(!armed){
    armed = true;
    analogWrite(motorControl, 0);
  }
  
  /*********************************
  Insert air brake code here
  ********************************/
  
  
  Serial.print("Accelerometer, ");
  Serial.print(accelerometer);
  Serial.print("; ");
  Serial.println(analogRead(accelerometer));
  
  //Waits for trigger condition, waits for programed delay and deploys airbrake.
  if(analogRead((accelerometer) < trigger)  && !deployed){
    delay(airbrakeDelay);
    analogWrite(motorControl, 255);
    delay(1000);
    analogWrite(motorControl, 0);
    deployed = true;
  }
  
  
  
  
  
  /*******************************
  End airbrake code
  *******************************/
}
