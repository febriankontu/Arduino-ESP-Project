#define relay1 15
#define relay2 2
#define relay3 4

void setup() {
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);

  Serial.begin(115200);
  Serial.println("Ketik A, B, atau C untuk menyalakan relay.");
}

void loop() {
  if (Serial.available()) {
    char data = Serial.read();  // Baca satu karakter saja

    switch (data) {
      case 'A':
      case 'a':
        digitalWrite(relay1, HIGH);
        Serial.println("Relay 1 ON");
        delay(500);
        digitalWrite(relay1, LOW);
        break;
      case 'B':
      case 'b':
        digitalWrite(relay2, HIGH);
        Serial.println("Relay 2 ON");
        delay(500);
        digitalWrite(relay2, LOW);
        break;
      case 'C':
      case 'c':
        digitalWrite(relay3, HIGH);
        Serial.println("Relay 3 ON");
        delay(500);
        digitalWrite(relay3, LOW);
        break;
      case 'X':
      case 'x':
        digitalWrite(relay1, LOW);
        digitalWrite(relay2, LOW);
        digitalWrite(relay3, LOW);
        Serial.println("Semua Relay OFF");
        break;
      // default:
      //   Serial.println("Karakter tidak dikenali");
      //   break;
    }
  }
}
