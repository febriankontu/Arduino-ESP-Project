#include <ezButton.h>

#define CLK1_PIN 25
#define DT1_PIN 26
#define SW1_PIN 27

#define CLK2_PIN 32
#define DT2_PIN 33

#define LIMIT_SWITCH_ATAS 5
#define LIMIT_SWITCH_BAWAH 18

#define red1 12
#define green1 13
#define red2 22
#define green2 23

const int COUNTER1_MIN = 0;
const int COUNTER1_MAX = 50;
const int COUNTER2_MIN = 0;
const int COUNTER2_MAX = 1000;

volatile int counter1 = 0;
volatile int counter2 = 0;

bool encoder1Enabled = false;
bool encoder2Enabled = false;

ezButton button(SW1_PIN);
unsigned long lastPrint = 0;

// === Interrupt Handler Encoder 1 ===
void IRAM_ATTR readEncoder1() {
  if (!encoder1Enabled) return;

  int dt = digitalRead(DT1_PIN);
  if (dt == HIGH) {
    counter1--;
  } else {
    counter1++;
  }

  if (counter1 < COUNTER1_MIN) counter1 = COUNTER1_MIN;
  if (counter1 > COUNTER1_MAX) counter1 = COUNTER1_MAX;
}

// === Interrupt Handler Encoder 2 ===
void IRAM_ATTR readEncoder2() {
  if (!encoder2Enabled) return;

  int dt = digitalRead(DT2_PIN);
  if (dt == HIGH) {
    counter2--;
  } else {
    counter2++;
  }

  if (counter2 < COUNTER2_MIN) counter2 = COUNTER2_MIN;
  if (counter2 > COUNTER2_MAX) counter2 = COUNTER2_MAX;
}

void setup() {
  Serial.begin(115200);

  pinMode(CLK1_PIN, INPUT_PULLUP);
  pinMode(DT1_PIN, INPUT_PULLUP);
  pinMode(CLK2_PIN, INPUT_PULLUP);
  pinMode(DT2_PIN, INPUT_PULLUP);
  pinMode(SW1_PIN, INPUT_PULLUP);

  pinMode(LIMIT_SWITCH_ATAS, INPUT_PULLUP);
  pinMode(LIMIT_SWITCH_BAWAH, INPUT_PULLUP);

  pinMode(red1, OUTPUT);
  pinMode(green1, OUTPUT);
  pinMode(red2, OUTPUT);
  pinMode(green2, OUTPUT);

  analogWrite(red1, 255);
  analogWrite(green1, 255);
  analogWrite(red2, 255);
  analogWrite(green2, 255);

  button.setDebounceTime(50);

  // Attach interrupt to CLK pins
  attachInterrupt(digitalPinToInterrupt(CLK1_PIN), readEncoder1, RISING);
  attachInterrupt(digitalPinToInterrupt(CLK2_PIN), readEncoder2, RISING);
}

void loop() {
  button.loop();

  // Limit Switch Logic
  if (digitalRead(LIMIT_SWITCH_ATAS) == HIGH) {
    counter1 = COUNTER1_MIN;
    counter2 = COUNTER2_MIN;
  }
  if (digitalRead(LIMIT_SWITCH_BAWAH) == HIGH) {
    counter1 = COUNTER1_MAX;
    counter2 = COUNTER2_MAX;
  }

  // Serial Command
  if (Serial.available()) {
    char cmd = Serial.read();
    if (cmd == 'a') {
      encoder1Enabled = true;
      encoder2Enabled = false;
      analogWrite(red1, 255);
      analogWrite(green1, 0);
      analogWrite(red2, 0);
      analogWrite(green2, 255);
    } else if (cmd == 'b') {
      encoder1Enabled = false;
      encoder2Enabled = true;
      analogWrite(red1, 0);
      analogWrite(green1, 255);
      analogWrite(red2, 255);
      analogWrite(green2, 0);
    } else if (cmd == 'c') {
      encoder1Enabled = true;
      encoder2Enabled = true;
      analogWrite(red1, 255);
      analogWrite(green1, 0);
      analogWrite(red2, 255);
      analogWrite(green2, 0);
    }
  }

  // Cetak tiap 100ms
  unsigned long now = millis();
  if (now - lastPrint >= 100) {
    lastPrint = now;

    int persen1 = map(counter1, COUNTER1_MIN, COUNTER1_MAX, 0, 100);
    int persen2 = map(counter2, COUNTER2_MIN, COUNTER2_MAX, 0, 100);

    Serial.print("ENC1: ");
    Serial.print(counter1);
    Serial.print(" (");
    Serial.print(persen1);
    Serial.print("%), ENC2: ");
    Serial.print(counter2);
    Serial.print(" (");
    Serial.print(persen2);
    Serial.println("%)");
  }

  if (button.isPressed()) {
    Serial.println("Tombol encoder 1 ditekan!");
  }
}
