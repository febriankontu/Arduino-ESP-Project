#include <ESP32Servo.h>
// #include <Servo.h>
#define in1 25
#define in2 26
#define in3 27
#define in4 14
#define enA 33
#define enB 12

Servo servo_lifting;
Servo servo_gripper;

// Gunakan channel PWM (0–15)
#define chA 0
#define chB 1
#define freq 1000   // frekuensi PWM
#define resolution 8 // 8-bit (0–255)



void setup() {
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  servo_lifting.attach(22);
  servo_gripper.attach(23);

  Serial.begin(115200);

  ledcSetup(chA, freq, resolution);
  ledcAttachPin(enA, chA);

  // Inisialisasi PWM di ESP32
  // ledcSetup(chA, freq, resolution);
  // ledcSetup(chB, freq, resolution);
  // ledcAttachPin(enA, chA);
  // ledcAttachPin(enB, chB);

  // // Motor maju
  // digitalWrite(in1, HIGH);
  // digitalWrite(in2, LOW);
  // digitalWrite(in3, HIGH);
  // digitalWrite(in4, LOW);

  // // Atur kecepatan 200/255
  // ledcWrite(chA, 255);
  // ledcWrite(chB, 255);
}

void loop() {
  // Motor tetap maju
  // servo_lifting.write(0);
  // delay(1000);
  // servo_lifting.write(270);
  // delay(1000);
  // servo_lifting.write(180);
  // delay(100);
  //  if (Serial.available() > 0) {
  //   int derajat = Serial.parseInt();
  //   // if (derajat >= 0 && derajat <= 180) {
  //     servo_gripper.write(derajat);
  //     Serial.print("Servo lifting ke: ");
  //     Serial.println(derajat);
    
  // }

  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  ledcWrite(chA, 255);
  delay(1000);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  ledcWrite(chA, 255);
  delay(1000);

  // servo_gripper.write(0);
  // delay(500);
  // servo_gripper.write(90);
  // delay(500);
  // servo_gripper.write(180);
  // delay(500);
  

  // servo_lifting.write(0);
  // delay(1000);
  // servo_lifting.write(180);
  // delay(1000);
}
