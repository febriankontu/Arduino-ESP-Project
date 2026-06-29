#include <RotaryEncoder.h>

#define pb1 8
#define pb2 9
#define pb3 10

RotaryEncoder enc(2, 4, RotaryEncoder::LatchMode::TWO03);

unsigned long lastCheck = 0;
long lastEnc = 0;
  
const int PPR = 2000; //Pulse per rotation
const float wheelDiameter = 0.06; //diameter roda
const float pi = 3.1416;
const float wheelCircum = pi * wheelDiameter; // 

void setup() {
  // put your setup code here, to run once:
  pinMode(pb1, INPUT_PULLUP);
  pinMode(pb2, INPUT_PULLUP);
  pinMode(pb3, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  enc.tick();
  unsigned long now = millis();
  int statePB1 = !digitalRead(pb1);
  int statePB2 = !digitalRead(pb2);
  int statePB3 = !digitalRead(pb3);

  // Hitung setiap 100 ms
  if (now - lastCheck >= 100) {
    long currentEnc = enc.getPosition();
    long delta = currentEnc - lastEnc;

    // km/jam
    float deltaRev = delta / (float)PPR;                   // perubahan putaran
    float distanceMeter = deltaRev * wheelCircum;          // jarak dalam meter
    float speedMps = distanceMeter / 0.1;                   // 0.1 = 100ms interval
    float speedKmph = speedMps * 3.6;                       // konversi ke km/h

    // if(speedKmph < 0){
    //   speedKmph = 0;
    // }
    
    // //RPM
    // float rpm = (delta / (float)PPR) * (600.0);  // 600 = 60 detik / 0.1 detik

    // if(rpm < 0){
    //   rpm = 0;
    // }
    
    Serial.print(speedKmph);     //Km/jam
    // Serial.print((int)rpm);      //RPM
    Serial.print(", ");
    Serial.print(statePB1);
    Serial.print(", ");
    Serial.print(statePB2);
    Serial.print(", ");
    Serial.print(statePB3);
    Serial.println(" ");
    lastEnc =  currentEnc;
    lastCheck = now;
  }

  // if(encPos != lastPos){
  //   Serial.println(encPos);
  //   lastPos = encPos;
  // }
  // delay(10);
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
