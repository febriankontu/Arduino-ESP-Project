#define encoderPin 6  // pin input encoder
const int PPR = 1; // Pulse Per Rotation (jumlah pulsa per 1 putaran)

volatile long pulseCount = 0;
bool lastState = LOW;

unsigned long lastRPMCheck = 0;
float rpm = 0;
float speedKmph = 0;
const float pi = 3.1416;
const float wheelDiameter = 0.13;
const float wheelCircum = pi * wheelDiameter;

void setup() {
  pinMode(encoderPin, INPUT);
  Serial.begin(9600);
}

void loop() {
  // --- Hitung pulsa manual (rising edge detection) ---
  bool currState = digitalRead(encoderPin);
  if (!lastState && currState) {
    pulseCount++;
  }
  lastState = currState;

  // --- Hitung RPM tiap 1 detik ---
  unsigned long now = millis();
  if (now - lastRPMCheck >= 500) {
    // noInterrupts();
    long pulses = pulseCount;
    // pulseCount = 0;
    // interrupts();

    // Hitung rotasi per menit
    rpm = (pulses / (float)PPR) * 120.0;
    float revolutions = pulses / (float)PPR;          // rotasi penuh
    float distanceMeters = revolutions * wheelCircum; // meter per detik
    float speedMps = distanceMeters / 1.0;             // per detik (karena 1000ms)
    // speedKmph = speedMps * 3.6;
    speedKmph = rpm * wheelCircum * 0.06; 
    String state = String(rpm) + ", " + String(speedKmph);
    Serial.println(state);
    lastRPMCheck = now;
  }
}
