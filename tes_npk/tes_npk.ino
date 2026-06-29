#include <Arduino.h>

// --- KONFIGURASI PIN UART 1 ---
// Pastikan wiring: 
// Pin 32 (RX ESP32) <--> RO/TXD Modul
// Pin 33 (TX ESP32) <--> DI/RXD Modul
#define RX_PIN 33
#define TX_PIN 32

HardwareSerial sensorSerial(1); 

// --- COMMAND REQUEST KHUSUS ---
// ID: 01, Func: 03, Start Addr: 00 1E (30), Length: 00 03 (3 data), CRC: 65 CD
const byte npkQuery[] = {0x01, 0x03, 0x00, 0x1E, 0x00, 0x03, 0x65, 0xCD};

void setup() {
  Serial.begin(115200); // Ke Laptop
  
  // Baudrate sensor biasanya 9600 (sesuai tabel gambar paling bawah "2 represents 9600")
  sensorSerial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
  
  Serial.println("Mulai Membaca Sensor NPK (Register 0x1E)...");
  delay(1000);
}

void loop() {
  // Bersihkan buffer serial sebelum kirim
  while (sensorSerial.available()) sensorSerial.read();

  Serial.print("Mengirim Request... ");
  sensorSerial.write(npkQuery, sizeof(npkQuery));
  
  // Tunggu balasan
  delay(200); 

  if (sensorSerial.available()) {
    byte values[11];
    // Kita harapkan balasan 11 byte:
    // ID, Func, Len, N_High, N_Low, P_High, P_Low, K_High, K_Low, CRC_L, CRC_H
    int bytesRead = sensorSerial.readBytes(values, 11);

    Serial.println("Data Masuk!");
    
    // DEBUG: Tampilkan HEX Mentah
    // Jika muncul "1 3 6 0 0 0 0 0 0 ..." artinya nilainya memang 0 dari sananya.
    Serial.print("RAW HEX: ");
    for (int i = 0; i < bytesRead; i++) {
      Serial.print(values[i], HEX);
      Serial.print(" ");
    }
    Serial.println();

    // Validasi Header (ID=1, Func=3, ByteCount=6)
    if(bytesRead >= 7 && values[0] == 0x01 && values[1] == 0x03 && values[2] == 0x06){
       
       int valNitrogen    = (values[3] << 8) | values[4];
       int valPhosphorus  = (values[5] << 8) | values[6];
       int valPotassium   = (values[7] << 8) | values[8];

       Serial.println("----- HASIL UKUR -----");
       Serial.print("Nitrogen (N)   : "); Serial.print(valNitrogen); Serial.println(" mg/kg");
       Serial.print("Phosphorus (P) : "); Serial.print(valPhosphorus); Serial.println(" mg/kg");
       Serial.print("Potassium (K)  : "); Serial.print(valPotassium); Serial.println(" mg/kg");
       Serial.println("----------------------");
       
       if(valNitrogen == 0 && valPhosphorus == 0 && valPotassium == 0) {
         Serial.println("INFO: Nilai 0 wajar untuk air biasa/tanah kering.");
         Serial.println("COBA: Celupkan ke air garam pekat atau air pupuk.");
       }

    } else {
      Serial.println("ERROR: Format data salah / CRC Error.");
    }
    
  } else {
    Serial.println("TIMEOUT: Tidak ada balasan dari sensor.");
    Serial.println("Cek: Kabel RX/TX terbalik? Power 12V masuk?");
  }

  delay(2000); 
}