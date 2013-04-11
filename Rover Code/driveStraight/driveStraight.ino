int driveR1 = 9;
int driveL1 = 10;
int driveR2 = 8;
int driveL2 = 11;
int rEnable = 5;
int lEnable = 6;

void setup(){
  for(int i = rEnable; i < (driveL2 + 1); i++){
    pinMode(i, OUTPUT);
  }
  
  digitalWrite(rEnable, HIGH);
  digitalWrite(lEnable, HIGH);
  
  digitalWrite(driveR1, HIGH);
  digitalWrite(driveL1, HIGH);
  digitalWrite(driveR2, LOW);
  digitalWrite(driveL2, LOW);
}

void loop(){}
