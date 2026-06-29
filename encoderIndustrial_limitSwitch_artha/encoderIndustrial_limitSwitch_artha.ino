#define encoderPinA 25
#define encoderPinB 26
#define limitSwitchAtas 5
#define limitSwitchBawah 18

volatile long encPos = 0;
long lastPos = 0;

unsigned long lastState = 0;

const int COUNTER_MIN = 0;
const int COUNTER_MAX = 4000;

void IRAM_ATTR updateEncA(){
  bool A = digitalRead(encoderPinA);
  bool B = digitalRead(encoderPinB);
  if (A == B) encPos++;
  else encPos--;
}

void IRAM_ATTR updateEncB(){
  bool A = digitalRead(encoderPinA);
  bool B = digitalRead(encoderPinB);
  if (A != B) encPos++;
  else encPos--;
}

void setup() {
  // put your setup code here, to run once:
  pinMode(encoderPinA, INPUT_PULLUP);
  pinMode(encoderPinB, INPUT_PULLUP);
  pinMode(limitSwitchAtas, INPUT_PULLUP);
  pinMode(limitSwitchBawah, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(encoderPinA), updateEncA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderPinB), updateEncB, CHANGE);

  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  int dataLimitAtas = digitalRead(limitSwitchAtas);
  int dataLimitBawah = digitalRead(limitSwitchBawah);

  if(dataLimitAtas == 1){
    encPos = COUNTER_MIN;
  }

  if(dataLimitBawah == 1){
    encPos = COUNTER_MAX;
  }

  int persen = map(encPos, COUNTER_MIN, COUNTER_MAX, 0, 100);

  unsigned long now = millis();

  if(now - lastState >= 100){
    lastState = now;

  if(encPos != lastPos){
    lastPos = encPos;
    }  
    Serial.print(encPos);
    Serial.print(", ");
    Serial.print(persen);
    Serial.println("%");
  }
}
