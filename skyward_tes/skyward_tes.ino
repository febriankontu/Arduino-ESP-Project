#include <Wire.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Adafruit_PN532.h>

// --- Pin dan perangkat ---
#define SDA_PIN 21
#define SCL_PIN 22
#define LIMIT_SWITCH_PIN 34
#define RELAY_PIN 13
#define PIN_CS 5
#define PIN_RST 16

// --- State sistem ---
enum SystemState {
  MENUNGGU_NFC,
  MENUNGGU_LIMIT
};

SystemState currentState = MENUNGGU_NFC;

// --- Ethernet setup ---
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress localIP(192, 168, 18, 251);
IPAddress serverIP(192, 168, 18, 252);
const uint16_t serverPort = 5000;

EthernetClient client;
bool isConnected = false;
unsigned long lastConnectAttempt = 0;
int connectRetryCount = 0;

// --- NFC ---
Adafruit_PN532 nfc(SDA_PIN, SCL_PIN);

// --- Heartbeat ---
unsigned long lastHeartbeat = 0;
unsigned long lastPrintTime = 0;
const unsigned long printInterval = 1000;

// --- Setup ---
void setup() {
  Serial.begin(115200);

  pinMode(LIMIT_SWITCH_PIN, INPUT_PULLUP);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  // --- Ethernet Init ---
  pinMode(PIN_RST, OUTPUT);
  digitalWrite(PIN_RST, LOW); delay(100);
  digitalWrite(PIN_RST, HIGH); delay(500);

  Ethernet.init(PIN_CS);
  Ethernet.begin(mac, localIP);
  delay(1000);

  Serial.print("ESP IP Address: ");
  Serial.println(Ethernet.localIP());

  // --- NFC Init ---
  Serial.println("Initializing PN532...");
  nfc.begin();
  uint32_t version = nfc.getFirmwareVersion();
  if (!version) {
    Serial.println("Didn't find PN532 board");
    while (1);
  }
  nfc.SAMConfig();
  Serial.println("PN532 ready!");
}

// --- Kirim UID ke server ---
bool kirimUIDkeServer(String deviceID, String uid) {
  if (!client.connected()) reconnectToServer();

  if (client.connected()) {
    String pesan = "|UID:" + uid;
    client.println(pesan);
    Serial.println("UID sent to server: " + pesan);
    return true;
  } else {
    Serial.println("Gagal mengirim UID - tidak terkoneksi.");
    return false;
  }
}

// --- Reconnect ---
void reconnectToServer() {
  if (millis() - lastConnectAttempt < 5000) return;
  lastConnectAttempt = millis();
  connectRetryCount++;

  Serial.printf("[RECONNECT] Attempt #%d to %s:%d\n", connectRetryCount, serverIP.toString().c_str(), serverPort);
  client.stop();
  client.setTimeout(10000);

  if (client.connect(serverIP, serverPort)) {
    Serial.println("[CONNECTED] Server connected.");
    isConnected = true;
    connectRetryCount = 0;
  } else {
    Serial.println("[FAILED] Could not connect to server.");
    isConnected = false;
  }
}

// --- Loop utama ---
void loop() {
  Ethernet.maintain();

  // Reconnect if disconnected
  if (!client.connected()) {
    isConnected = false;
    reconnectToServer();
  }

  // Kirim heartbeat setiap 30 detik
  if (millis() - lastHeartbeat > 30000 && client.connected()) {
    client.println("HEARTBEAT");
    lastHeartbeat = millis();
  }

  switch (currentState) {

    case MENUNGGU_NFC: {
      if (millis() - lastPrintTime > printInterval) {
        Serial.println("Waiting for NFC tag...");
        lastPrintTime = millis();
      }

      uint8_t uid[7];
      uint8_t uidLength;
      if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
        Serial.println("NFC tag detected!");

        String uidStr = "";
        for (int i = 0; i < uidLength; i++) {
          uidStr += String(uid[i]);
          if (i < uidLength - 1) uidStr += ".";
        }

        // Coba kirim UID ke server 
        bool sukses = kirimUIDkeServer("ESP1", uidStr);

        if (sukses) {
          currentState = MENUNGGU_LIMIT;
        } else {
          Serial.println("Gagal mengirim. Silakan tempelkan kartu lagi.");
          // Kembali ke MENUNGGU_NFC
        }
      }
      break;
    }

    case MENUNGGU_LIMIT: {
      if (millis() - lastPrintTime > printInterval) {
        Serial.println("Waiting for limit switch press...");
        lastPrintTime = millis();
      }

      if (digitalRead(LIMIT_SWITCH_PIN) == LOW) {
        digitalWrite(RELAY_PIN, LOW);
        delay(300);
        digitalWrite(RELAY_PIN, HIGH);
        Serial.println("Limit switch pressed. Back to NFC mode.\n");
        currentState = MENUNGGU_NFC;
      }
      break;
    }
  }

  delay(50);
}
