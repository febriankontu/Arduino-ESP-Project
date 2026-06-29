#include <TinyGPSPlus.h>
#include <HardwareSerial.h>

TinyGPSPlus gps;
HardwareSerial gpsSerial(2);

// --- KONFIGURASI ZONA WAKTU ---
const int timeZoneOffset = 7; // Ganti 7 (WIB), 8 (WITA), atau 9 (WIT)

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(9600, SERIAL_8N1, 16, 17);
  Serial.println("Memulai pembacaan GPS... Menunggu satelit.");
}

void loop() {
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());

    if (gps.location.isUpdated()) {
      Serial.println("===== DATA GPS =====");
      
      // --- PERBAIKAN WAKTU (Manual Adjustment) ---
      int jamLokal = gps.time.hour() + timeZoneOffset;
      int menit = gps.time.minute();
      int detik = gps.time.second();
      
      // Koreksi jika jam melebihi 24 (Pindah hari)
      if (jamLokal >= 24) {
        jamLokal = jamLokal - 24;
        // Catatan: Tanggal juga seharusnya maju 1 hari, 
        // tapi logika tanggal manual cukup rumit.
        // Untuk keperluan jam saja, ini sudah cukup.
      }
      // Koreksi jika hasilnya negatif (jarang terjadi kecuali offset minus)
      if (jamLokal < 0) {
        jamLokal = jamLokal + 24;
      }

      Serial.print("Waktu Lokal : ");
      if (jamLokal < 10) Serial.print("0"); // Tambah nol di depan jika satuan
      Serial.print(jamLokal); Serial.print(":");
      
      if (menit < 10) Serial.print("0");
      Serial.print(menit); Serial.print(":");
      
      if (detik < 10) Serial.print("0");
      Serial.println(detik);
      
      // Data Lainnya
      Serial.print("Latitude    : "); Serial.println(gps.location.lat(), 6);
      Serial.print("Longitude   : "); Serial.println(gps.location.lng(), 6);
      Serial.println("====================\n");
    }
  }
}