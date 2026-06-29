#define encoderPinA 6
#define pb1 15
#define pb2 5
#define pb3 4

volatile long encPos = 0;
long lastPos = 0;

unsigned long lastCheck = 0;
long lastEnc = 0;
const int PPR = 2000; //Pulse per rotation
const float wheelDiameter = 0.1; //diameter roda
const float pi = 3.1416;
const float wheelCircum = pi * wheelDiameter; // 

// void IRAM_ATTR updateEnc() {
//   int A = digitalRead(encoderPinA);
//   int B = digitalRead(encoderPinB);
//   encPos += (A == B) ? 1 : -1;
// }

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
  pinMode(pb1, INPUT_PULLUP);
  pinMode(pb2, INPUT_PULLUP);
  pinMode(pb3, INPUT_PULLUP);
  digitalWrite(encoderPinA, HIGH);
  digitalWrite(encoderPinB, HIGH);
  // pinMode(zIndex, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(encoderPinA), updateEncA, CHANGE);
  // attachInterrupt(digitalPinToInterrupt(encoderPinB), updateEncB, CHANGE);
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long now = millis();
  // float speedKmph;
  // // // Hitung setiap 100 ms
  // if (now - lastCheck >= 100) {
  //   long currentEnc;
  //   int statePB1 = !digitalRead(pb1);
  //   int statePB2 = !digitalRead(pb2);
  //   int statePB3 = !digitalRead(pb3);
  //   noInterrupts();
  //   currentEnc = encPos;
  //   interrupts();

  //   long delta = currentEnc - lastEnc;

  //   if (delta < 0 || delta > PPR * 30) {
  //     delta = 0;
  //   }

  //   //RPM
  //   float rpm = (delta / (float)PPR) * (600.0);  // 600 = 60 detik / 0.1 detik

  //   // km/jam
  //   // float deltaRev = delta / (float)PPR;                   // perubahan putaran
  //   // float distanceMeter = deltaRev * wheelCircum;          // jarak dalam meter
  //   // float speedMps = distanceMeter / 0.25;                 // 0.1 = 100ms interval
  //   speedKmph = rpm * wheelCircum * 0.06;                // konversi ke km/h
  //   // speedKmph = ((delta / (float)PPR) * wheelCircum / 0.01) * 3.6;

  //   String state = String((int)rpm) + ", " + String(speedKmph) +  ", " + String(statePB1) + ", " +  String(statePB2) +  ", " + String(statePB3);
  //   Serial.println(state);
  //  if(encPos != lastPos){
  //   Serial.println(encPos);
  //   lastPos = encPos;
  // }
    encPos = 0;
    lastEnc =  currentEnc;
    lastCheck = now;
  } 
}



// void updateEnc(){
//   int stateA = digitalRead(encoderPinA);
//   int stateB = digitalRead(encoderPinB);

//   if(stateA == stateB){
//     encPos++;
//   }
//   else{
//     encPos--;
//   }
// }



