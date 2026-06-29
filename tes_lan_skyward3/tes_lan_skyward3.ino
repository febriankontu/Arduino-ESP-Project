#include <SPI.h>
#include <Ethernet.h>
#include <Adafruit_PN532.h>

#define SDA_PIN 21
#define SCL_PIN 22

// W5500 config (ubah sesuai konfigurasi kamu)
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 18, 100);  // IP statis ESP32
IPAddress dns(8, 8, 8, 8);       // DNS server (Google DNS)

// Server configuration menggunakan DNS
const char* serverHost = "http://skyward.arthatronic.com/api";  // Hostname/domain
int serverPort = 3130;  // Ubah ke port 80 untuk HTTP standar

// NFC via I2C
Adafruit_PN532 nfc(SDA_PIN, SCL_PIN);

// Ethernet Client
EthernetClient client;

void setup() {
  Serial.begin(115200);

  // Initialize Ethernet dengan DNS server
  Ethernet.begin(mac, ip, dns);
  delay(1000);
  Serial.println("Ethernet initialized");
  
  // Print network info
  Serial.print("IP Address: ");
  Serial.println(Ethernet.localIP());
  Serial.print("DNS Server: ");
  Serial.println(Ethernet.dnsServerIP());

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
  uint8_t uid[7]; 
  uint8_t uidLength;

  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
    String uidStr = "";
    for (uint8_t i = 0; i < uidLength; i++) {
      if (uid[i] < 0x10) uidStr += "0";  // Tambahkan leading zero
      uidStr += String(uid[i], HEX);     // Format sebagai HEX
    }
    uidStr.toUpperCase();  // Konversi ke uppercase

    Serial.println("UID: " + uidStr);

    // === Koneksi ke Flask Server menggunakan hostname ===
    Serial.print("Connecting to ");
    Serial.print(serverHost);
    Serial.print(":");
    Serial.println(serverPort);
    
    if (!client.connect(serverHost, serverPort)) {
      Serial.println("Gagal terhubung ke server");
      delay(3000);
      return;
    }

    // Buat request HTTP yang benar
    String url = "/api/v1/user?id=RFID-" + uidStr + "&isLast=false";
    String authHeader = "Authorization: Basic c2t5d2FyZDpiaXgtZ2RpcXchZ3N3aWdxaSpqaHNkb3F3aCMhaHNnZGc=";

    // Kirim request HTTP
    client.println("GET " + url + " HTTP/1.1");
    client.println("Host: " + String(serverHost));  // Header Host wajib untuk HTTP/1.1
    client.println(authHeader);
    client.println("Connection: close");
    client.println();

    Serial.println("Request sent: GET " + url);

    // === Baca respon server ===
    bool allowed = false;
    String response = "";
    
    // Tunggu response
    unsigned long timeout = millis();
    while (client.connected() && millis() - timeout < 10000) {
      if (client.available()) {
        String line = client.readStringUntil('\n');
        Serial.println(line);
        response += line;
        
        // Cek apakah user diauthorisasi (sesuaikan dengan response server Anda)
        if (line.indexOf("authorized") >= 0 || line.indexOf("\"status\":\"success\"") >= 0) {
          allowed = true;
        }
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