#include <SPI.h>
#include <Ethernet.h>
#include <Adafruit_PN532.h>

#define SDA_PIN 21
#define SCL_PIN 22

// W5500 config (ubah sesuai konfigurasi kamu)
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 18, 100);  // IP statis ESP32
IPAddress gateway(192, 168, 18, 1);  // Gateway router
IPAddress subnet(255, 255, 255, 0);  // Subnet mask
IPAddress dns(8, 8, 8, 8);           // DNS server (Google DNS)

// Server configuration - UBAH SESUAI SERVER ANDA
const char* serverHost = "skyward.arthatronic.com";  // Server domain
int serverPort = 3130;  // Port HTTP default (tidak perlu diubah)

// NFC via I2C
Adafruit_PN532 nfc(SDA_PIN, SCL_PIN);

// Ethernet Client
EthernetClient client;

void setup() {
  Serial.begin(115200);
  delay(2000);  // Wait for serial monitor

  Serial.println("=== ESP32 NFC Reader Starting ===");

  // Initialize Ethernet dengan konfigurasi lengkap
  Ethernet.begin(mac, ip, dns, gateway, subnet);
  delay(2000);
  
  // Print network info untuk debugging
  Serial.println("=== Network Configuration ===");
  Serial.print("IP Address: ");
  Serial.println(Ethernet.localIP());
  Serial.print("Gateway: ");
  Serial.println(Ethernet.gatewayIP());
  Serial.print("Subnet: ");
  Serial.println(Ethernet.subnetMask());
  Serial.print("DNS Server: ");
  Serial.println(Ethernet.dnsServerIP());
  
  // Test network connectivity
  Serial.println("=== Testing Network Connectivity ===");
  testConnectivity();

  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println("Tak dapat mendeteksi PN532");
    while (1);  // Hentikan
  }

  nfc.SAMConfig();
  Serial.println("Tempelkan kartu untuk verifikasi...");
}

// Function untuk test konektivitas network
void testConnectivity() {
  Serial.println("Testing DNS resolution...");
  
  // Test DNS resolution
  IPAddress resolvedIP;
  int result = Ethernet.dnsLookup(serverHost, resolvedIP);
  
  if (result == 1) {
    Serial.print("DNS Resolution SUCCESS: ");
    Serial.print(serverHost);
    Serial.print(" -> ");
    Serial.println(resolvedIP);
  } else {
    Serial.print("DNS Resolution FAILED for: ");
    Serial.println(serverHost);
    Serial.println("Check your DNS server or use IP address instead");
  }
  
  // Test basic connectivity ke Google
  Serial.println("Testing basic internet connectivity...");
  EthernetClient testClient;
  if (testClient.connect("google.com", 80)) {
    Serial.println("Internet connectivity: OK");
    testClient.stop();
  } else {
    Serial.println("Internet connectivity: FAILED");
    Serial.println("Check network configuration");
  }
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

    // === Koneksi ke Server menggunakan hostname ===
    Serial.println("=== Connecting to Server ===");
    Serial.print("Attempting to connect to: ");
    Serial.print(serverHost);
    Serial.print(":");
    Serial.println(serverPort);
    
    // Coba resolve DNS dulu
    IPAddress serverIP;
    int dnsResult = Ethernet.dnsLookup(serverHost, serverIP);
    
    if (dnsResult == 1) {
      Serial.print("DNS resolved to: ");
      Serial.println(serverIP);
    } else {
      Serial.println("DNS resolution failed!");
      delay(3000);
      return;
    }
    
    if (!client.connect(serverHost, serverPort)) {
      Serial.println("❌ GAGAL terhubung ke server");
      Serial.println("Possible causes:");
      Serial.println("1. Server tidak aktif");
      Serial.println("2. Firewall blocking connection");
      Serial.println("3. Port salah");
      Serial.println("4. Domain tidak exist");
      delay(3000);
      return;
    }
    
    Serial.println("✅ Connected to server!");

    // Buat request HTTP dengan path lengkap /api
    String url = "/api/v1/user?id=RFID-" + uidStr + "&isLast=false";  // Path lengkap dengan /api
    
    String authHeader = "Authorization: Basic c2t5d2FyZDpiaXgtZ2RpcXchZ3N3aWdxaSpqaHNkb3F3aCMhaHNnZGc=";

    // Kirim request HTTP
    client.println("GET " + url + " HTTP/1.1");
    client.println("Host: " + String(serverHost));
    client.println(authHeader);
    client.println("User-Agent: ESP32-NFC-Reader");
    client.println("Accept: application/json");
    client.println("Connection: close");
    client.println();

    Serial.println("📤 Request sent: GET http://" + String(serverHost) + url);

    // === Baca respon server ===
    Serial.println("📥 Waiting for response...");
    bool allowed = false;
    String response = "";
    String httpStatus = "";
    
    // Tunggu response dengan timeout
    unsigned long timeout = millis();
    while (client.connected() && millis() - timeout < 10000) {
      if (client.available()) {
        String line = client.readStringUntil('\n');
        line.trim();  // Remove whitespace
        
        if (line.startsWith("HTTP/")) {
          httpStatus = line;
          Serial.println("📋 HTTP Status: " + httpStatus);
        }
        
        Serial.println("📄 " + line);
        response += line;
        
        // Cek apakah user diauthorisasi (SESUAIKAN DENGAN RESPONSE SERVER ANDA)
        if (line.indexOf("authorized") >= 0 || 
            line.indexOf("\"status\":\"success\"") >= 0 ||
            line.indexOf("\"access\":\"granted\"") >= 0) {
          allowed = true;
        }
      }
    }
    
    if (millis() - timeout >= 10000) {
      Serial.println("⏰ Response timeout!");
    }

    client.stop();
    Serial.println("🔌 Connection closed");

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