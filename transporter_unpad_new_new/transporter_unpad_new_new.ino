#include <PS4Controller.h>
#include <ESP32Servo.h>

#define IN1 33
#define IN2 25
#define IN3 27
#define IN4 14
#define ENA 26
#define ENB 12

Servo servo_lifting;
Servo servo_gripper;

#define CH_A 0
#define CH_B 1
#define FREQ 1000
#define RESOLUTION 8
#define DEADZONE 15

int moveValue = 0;
int rotateValue = 0;

// ====================== SETUP ======================
void setup() {
  Serial.begin(115200);
  PS4.begin("38:4f:f0:8c:56:2f");
  Serial.println("PS4 Controller Ready with ps4SetEventCallback!");

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  servo_lifting.attach(2, 500, 2400);
  servo_gripper.attach(15, 500, 2400);

  ledcSetup(CH_A + 4, FREQ, RESOLUTION);
  ledcSetup(CH_B + 4, FREQ, RESOLUTION);
  ledcAttachPin(ENA, CH_A + 4);
  ledcAttachPin(ENB, CH_B + 4);

  // Daftarkan callback utama
 PS4.attach(ps4Event);
}

// ====================== CALLBACK PS4 EVENT ======================
void ps4Event() {
// void ps4Event(ps4_t ps4, ps4_event_t event) {
  // ======== 1. Analognya bergerak ========
  // if (event.analog_move.stick.lx != 0 || event.analog_move.stick.ly != 0 ||
  //     event.analog_move.stick.rx != 0 || event.analog_move.stick.ry != 0) {
    
  //   moveValue = antiDrift(ps4.analog.stick.ly, DEADZONE);
  //   rotateValue = antiDrift(ps4.analog.stick.rx, DEADZONE);

  //   moveValue = map(moveValue, -128, 128, -255, 255);
  //   rotateValue = map(rotateValue, -128, 128, -190, 190);
  //   controlRobot(moveValue, rotateValue);
  // }

  // // ======== 2. Tombol ditekan ========
  // if (event.button_down.square) {
  //   servo_gripper.write(0);
  //   Serial.println("🟩 Gripper buka");
  // }
  // else if (event.button_down.cross) {
  //   servo_gripper.write(180);
  //   Serial.println("❌ Gripper tutup");
  // }
  // else{
  //   servo_gripper.write(90);
  // }

  // if (event.button_down.r1) {
  //   servo_lifting.write(180);
  //   Serial.println("⬆️ Lifting naik");
  // }
  // else if (event.button_down.l1) {
  //   servo_lifting.write(0);
  //   Serial.println("⬇️ Lifting turun");
  // }
  // else{
  //   servo_lifting.write(90);
  // }
  // if (event.button_down.ps) {
  //   stopMotor();
  //   Serial.println("🚨 EMERGENCY STOP!");
  // }

  moveValue = antiDrift(PS4.RStickX(), DEADZONE);
  moveValue = map(moveValue, -128, 128, -190, 190);

  // Baca nilai analog dari stick kanan (putar kiri/kanan)
  rotateValue = antiDrift(PS4.LStickY(), DEADZONE);
  rotateValue = map(rotateValue, -128, 128, -255, 255);

  // Kendali robot
  controlRobot(moveValue, rotateValue);

  // digitalWrite(IN1, HIGH);
  // digitalWrite(IN2, LOW);
  // ledcWrite(CH_A, 220);

  //Kendali servo gripper
  // if (PS4.Triangle()){
  //   servo_gripper.write(0);
  //   Serial.println("0 derajat");
  // }
  if (PS4.Square()){
    servo_gripper.write(30);
    Serial.println("Buka");
  }
  // else if (PS4.Cross()){
  //   servo_gripper.write(180);
  //   Serial.println("Tutup");
  // }
  else{
    servo_gripper.write(105);
  }


  //kendali servo lifting
  if(PS4.R1()){
    servo_lifting.write(180);
    Serial.println("naik");  
  }
  else if(PS4.L1()){
    servo_lifting.write(0);
    Serial.println("turun");
  }
  else{
    servo_lifting.write(90);
    Serial.println("berhenti");
  }


  // Debug monitor
  Serial.print("Move: ");
  Serial.print(moveValue);
  Serial.print("\tRotate: ");
  Serial.println(rotateValue);

    // delay(50);




  // ======== 3. Tombol dilepas ========
  // if (event.button_down.square || event.button_down.cross) {
  //   servo_gripper.write(90);
  //   Serial.println("Gripper netral");
  // }
  // if (event.button_down.l1 || event.button_down.r1) {
  //   servo_lifting.write(90);
  //   Serial.println("Lifting berhenti");
  // }
}

// ====================== LOOP (kosong, semua lewat callback) ======================
void loop() {
  // Semua input ditangani di ps4Event()
}

// ====================== FUNCTION TAMBAHAN ======================
int antiDrift(int value, int threshold) {
  if (abs(value) < threshold) return 0;
  return value > 0 ? value - threshold : value + threshold;
}

void controlRobot(int move, int rotate) {
  int leftMotor = constrain(-move + rotate, -255, 255);
  int rightMotor = constrain(-move - rotate, -255, 255);

  // Motor kiri
  if (leftMotor > 0) {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    ledcWrite(CH_B + 4, leftMotor);
  } else if (leftMotor < 0) {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    ledcWrite(CH_B + 4, abs(leftMotor));
  } else {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    ledcWrite(CH_B + 4, 0);
  }

  // Motor kanan
  if (rightMotor > 0) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    ledcWrite(CH_A + 4, rightMotor);
  } else if (rightMotor < 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    ledcWrite(CH_A + 4, abs(rightMotor));
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    ledcWrite(CH_A + 4, 0);
  }
}

void stopMotor() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  ledcWrite(CH_A + 4, 0);
  ledcWrite(CH_B + 4, 0);
}
