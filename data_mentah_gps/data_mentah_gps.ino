HardwareSerial GPS(2);  // pakai UART2

void setup() {
  Serial.begin(115200);
  GPS.begin(9600, SERIAL_8N1, 16, 17);  // RX=16, TX=17
  Serial.println("Reading raw GPS data...");
}

void loop() {
  while (GPS.available()) {
    char c = GPS.read();
    Serial.print(c);   // tampilkan apa adanya
  }
}
