#include <TinyGPSPlus.h>
#include <HardwareSerial.h>

TinyGPSPlus gps;              // Buat objek GPS
HardwareSerial gpsSerial(2);  // Gunakan UART2 ESP32 (GPIO16,17)

void setup() {
  Serial.begin(115200);                 // Serial Monitor
  gpsSerial.begin(9600, SERIAL_8N1, 16, 17); // RX=16, TX=17
  Serial.println("Memulai pembacaan GPS NEO-6M...");
}

void loop() {
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());  // Parsing data GPS
    
    if (gps.location.isUpdated()) {
      Serial.println("===== DATA GPS =====");
      Serial.print("Latitude : "); Serial.println(gps.location.lat(), 6);
      Serial.print("Longitude: "); Serial.println(gps.location.lng(), 6);
      Serial.print("Altitude : "); Serial.print(gps.altitude.meters()); Serial.println(" m");
      Serial.print("Satellites: "); Serial.println(gps.satellites.value());
      Serial.print("HDOP      : "); Serial.println(gps.hdop.hdop());
      
      Serial.print("Tanggal   : ");
      Serial.print(gps.date.day()); Serial.print("/");
      Serial.print(gps.date.month()); Serial.print("/");
      Serial.println(gps.date.year());
      
      Serial.print("Waktu (UTC): ");
      Serial.print(gps.time.hour()); Serial.print(":");
      Serial.print(gps.time.minute()); Serial.print(":");
      Serial.println(gps.time.second());
      Serial.println("====================\n");
    }
  }
}
