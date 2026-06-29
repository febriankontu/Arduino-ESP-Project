#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoJson.h>

// SPI pin definitions for ESP32 (VSPI)
#define SPI_MOSI 23
#define SPI_MISO 19
#define SPI_SCK  18
#define SPI_CS   5  // W5500 CS pin

// Ethernet and SPI setup
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };  // random MAC
EthernetClient client;
SPIClass SPI_ETH(VSPI_HOST);

// Server info
const char* host = "skyward.arthatronic.com";
String url = "/api/v1/user?id=3978315&isLast=false";

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Start SPI bus for Ethernet
  SPI_ETH.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SPI_CS);
  Ethernet.init(SPI_CS);  // Set CS pin for Ethernet
  Ethernet.begin(mac, SPI_ETH);  // DHCP mode

  delay(2000);  // Give time for DHCP

  Serial.print("Ethernet status: ");
  if (Ethernet.linkStatus() == LinkON) {
    Serial.println("Connected");
  } else {
    Serial.println("Disconnected");
  }

  Serial.print("My IP: ");
  Serial.println(Ethernet.localIP());

  if (Ethernet.localIP() == IPAddress(255, 255, 255, 255)) {
    Serial.println("ERROR: Failed to obtain IP address via DHCP.");
    return;
  }

  if (client.connect(host, 80)) {
    Serial.println("Connected to server");

    // Send HTTP GET request
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
  } else {
    Serial.println("Connection to server failed.");
  }
}

void loop() {
  static bool headersEnded = false;
  static String jsonStr = "";

  while (client.connected() || client.available()) {
    String line = client.readStringUntil('\n');

    if (!headersEnded) {
      if (line == "\r") {
        headersEnded = true;  // Headers ended
      }
    } else {
      jsonStr += line;
    }
  }

  if (jsonStr.length() > 0) {
    Serial.println("Received JSON:");
    Serial.println(jsonStr);

    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, jsonStr);

    if (error) {
      Serial.print("JSON Parse Error: ");
      Serial.println(error.c_str());
    } else {
      if (doc.containsKey("status")) {
        const char* status = doc["status"];
        Serial.print("Status: ");
        Serial.println(status);
      } else {
        Serial.println("Key 'status' not found.");
      }
    }
  }

  // Optional: repeat every 30s
  delay(10000);


}