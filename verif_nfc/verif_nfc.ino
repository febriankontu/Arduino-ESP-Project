#include <SPI.h>
#include <Ethernet.h>
#include <Adafruit_PN532.h>

#define SDA_PIN 21
#define SCL_PIN 22

// W5500 config (ubah sesuai konfigurasi kamu)
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 18, 100);  // IP statis ESP32
IPAddress serverIP(13, 229, 120, 228);  // IP PC Flask server
int serverPort = 3130;

// NFC via I2C
Adafruit_PN532 nfc(SDA_PIN, SCL_PIN);

// Ethernet Client
EthernetClient client;

void setup() {
  Serial.begin(115200);

  Ethernet.begin(mac, ip);
  delay(1000);
  Serial.println("Ethernet initialized");

  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println("Tak dapat mendeteksi PN532");
    while (1);  // Hentikan
  }

  nfc.SAMConfig();
  Serial.println("Tempelkan kartu untuk verifikasi...");
}

void loop() {
  uint8_t uid[7]; uint8_t uidLength;

  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
    String uidStr = "";
    for (uint8_t i = 0; i < uidLength; i++) {
      uidStr += String(uid[i]);
  
    }

    Serial.println("UID: " + uidStr);

    // === Koneksi ke Flask Server ===
    if (!client.connect(serverIP, serverPort)) {
      Serial.println("Gagal terhubung ke server Flask");
      delay(3000);
      return;
    }

    String url = "/check?uid=" + uidStr;
    String authHeader = "Authorization: Basic c2t5d2FyZDpiaXctZ2RpcXchZ3N3aWdxaSpqaHNkb3F3aCMhaHNnZGc=";

    // Kirim request HTTP
    client.println("GET " + url + " HTTP/1.1");
    client.println("http://skyward.arthatronic.com/api/v1/user?id=RFID-20250618-04564&isLast=false");  // Sesuaikan dengan server
    client.println(authHeader);
    client.println("Connection: close");
    client.println();

    // === Baca respon server ===
    bool allowed = false;
    while (client.connected()) {
      String line = client.readStringUntil('\n');
      Serial.println(line);
      if (line.indexOf("authorized") >= 0) {
        allowed = true;
      }
    }

    client.stop();

    // === Hasil verifikasi UID ===
    if (allowed) {
      Serial.println("✅ UID Dikenali - Akses Diberikan");
      // nyalakan relay di sini jika perlu
    } else {
      Serial.println("❌ UID Tidak Terdaftar - Akses Ditolak");
    }

    delay(3000);  // tunggu sebelum baca ulang
  }
}
