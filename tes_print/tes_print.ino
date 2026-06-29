#include <SoftwareSerial.h>

// Ubah ke pin 5 (RX) dan 6 (TX)
//SoftwareSerial mySerial(10, 11);  // RX (printer TX) ke pin 5, TX (printer RX) ke pin 6

void setup() {
  Serial1.begin(9600);  // atau 19200 jika printer kamu mendukung
}

void loop() {
  Serial1.println("DFROBOT");
  Serial1.println();
  Serial1.println();
  Serial1.println();
  delay(10000);  // Tunggu 10 detik sebelum cetak lagi
}
