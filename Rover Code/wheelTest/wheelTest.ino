int r2=9;
int r1=8;
int l2=10;
int l1=11;

int re=6;
int le=5;

void setup(){
  for(int i = re; i < (l2 +1); i++){
    pinMode(i, OUTPUT);
  }
}

void loop(){
  digitalWrite(r1, HIGH);
  digitalWrite(l1, HIGH);
  digitalWrite(r2, LOW);
  digitalWrite(l2, LOW);
  
  digitalWrite(re, HIGH);
  digitalWrite(le, HIGH);
  
  
}

