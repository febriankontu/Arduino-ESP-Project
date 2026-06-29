#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>
#include <Adafruit_PN532.h>
#include <ArduinoJson.h>
#include <ArduinoBearSSL.h> // Pustaka PENTING untuk HTTPS

#define SDA_PIN 21
#define SCL_PIN 22

Adafruit_PN532 nfc(SDA_PIN, SCL_PIN);

// --- KONFIGURASI JARINGAN (WAJIB DIPERIKSA ULANG!) ---
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 18, 150);       // IP statis perangkat Arduino Anda
IPAddress gateway(192, 168, 18, 1);   // GANTI DENGAN IP GATEWAY ASLI ANDA
IPAddress dns(8, 8, 8, 8);            // GANTI DENGAN IP DNS ASLI ANDA

// --- KONFIGURASI SERVER ---
char server[] = "skyward.arthatronic.com"; // Nama domain saja
int port = 443;                             // Port untuk HTTPS

// Gunakan EthernetSSLClient untuk HTTPS
EthernetSSLClient client;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("Inisialisasi Ethernet...");
  Ethernet.begin(mac, ip, dns, gateway);
  
  delay(1000);
  Serial.print("IP Address Arduino: ");
  Serial.println(Ethernet.localIP());

  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println("Tak dapat mendeteksi PN532");
    while (1);
  }

  nfc.SAMConfig();
  Serial.println("\nTempelkan kartu untuk registrasi...");
}

void loop() {
  uint8_t uid[7];
  uint8_t uidLength;

  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
    // Format UID menjadi string Desimal yang digabung
    String uidStr = "";
    for (uint8_t i = 0; i < uidLength; i++) {
      uidStr += String(uid[i]);
    }

    Serial.print("UID terdeteksi (Decimal): ");
    Serial.println(uidStr);

    String postData;
    StaticJsonDocument<200> doc;
    doc["uid"] = uidStr;
    serializeJson(doc, postData);

    Serial.println("Menghubungkan ke server (HTTPS)...");
    if (client.connect(server, port)) {
      Serial.println("Terhubung ke server. Kirim POST...");
      
      client.println("POST /api/v1/user HTTP/1.1");
      client.print("Host: ");
      client.println(server);
      client.println("Content-Type: application/json");
      client.println("Authorization: Basic c2t5d2FyZDpiaXctZ2RpcXchZ3N3aWdxaSpqaHNkb3F3aCMhaHNnZGc=");
      client.print("Content-Length: ");
      client.println(postData.length());
      client.println("Connection: close");
      client.println();
      client.println(postData);

      // ... sisa kode untuk membaca respon ...
      Serial.println("\n--- Respon Server ---");
      while(client.connected()) {
        while(client.available()){
          char c = client.read();
          Serial.print(c);
        }
      }
      Serial.println("\n--- Akhir Respon ---");
      client.stop();

    } else {
      Serial.println("Gagal terhubung ke server.");
    }

    Serial.println("\nMenunggu kartu berikutnya...");
    delay(3000);
  }
}