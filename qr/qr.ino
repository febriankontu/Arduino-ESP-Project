#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <ArduinoJson.h>
#include <qrcode_gen.h>  // Pastikan file qrcodegen.c dan .h ada di folder proyek

// --- WiFi dan API ---
const char* ssid = "Arthatronic";
const char* password = "animatronics";

const String apiKey = "eG5kX3Byb2R1Y3Rpb25fd21RWVhhRXREeWZwS0tpT29VaEowMzluS0dBYWUwdmxOV3Fja0tkZVN6eWJ6Smw3UnJpYVdocHpwOTBleWRR";
const String referenceId = "test_id_123";

// --- Hardware ---
#define BUTTON_PIN 18
TFT_eSPI tft = TFT_eSPI();

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // WiFi Connect
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Display Init
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("Getting QR...");

  // Ambil dan tampilkan QR pertama
  String qrText = getQRCode();
  if (!qrText.isEmpty()) {
    displayQRCode(qrText);
  } else {
    displayError("Failed to get QR");
  }
}

void loop() {
  static bool lastButton = HIGH;
  bool nowButton = digitalRead(BUTTON_PIN);

  if (lastButton == HIGH && nowButton == LOW) {
    Serial.println("Button pressed, refreshing QR...");
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(10, 10);
    tft.println("Refreshing QR...");

    String qrText = getQRCode();
    if (!qrText.isEmpty()) {
      displayQRCode(qrText);
    } else {
      displayError("Failed to refresh QR");
    }

    delay(500); // debounce
  }

  lastButton = nowButton;
}

String getQRCode() {
  HTTPClient http;
  http.begin("https://api.xendit.co/qr_codes");
  http.setTimeout(5000);

  http.addHeader("Authorization", "Basic " + apiKey);
  http.addHeader("api-version", "2022-07-31");
  http.addHeader("Content-Type", "application/json");

  String payload = "{\"reference_id\": \"" + referenceId + "\", \"type\": \"DYNAMIC\", \"currency\": \"IDR\", \"amount\": 10}";
  int code = http.POST(payload);

  Serial.print("HTTP Code: ");
  Serial.println(code);
  String response = http.getString();
  Serial.println(response);

  if (code == 200 || code == 201) {
    StaticJsonDocument<1024> doc;
    DeserializationError err = deserializeJson(doc, response);
    if (err) {
      Serial.print("JSON Error: ");
      Serial.println(err.c_str());
      return "";
    }
    return doc["qr_string"] | "";
  }

  return "";
}

void displayQRCode(String text) {
  QRCode qrcode;
  const uint8_t version = 3;
  int size = qrcode_getBufferSize(version);
  uint8_t* buffer = (uint8_t*)malloc(size);

  if (!buffer) {
    displayError("QR alloc failed");
    return;
  }

  qrcode_initText(&qrcode, buffer, version, ECC_LOW, text.c_str());

  int scale = 5;
  int offsetX = (tft.width() - qrcode.size * scale) / 2;
  int offsetY = (tft.height() - qrcode.size * scale) / 2;

  tft.fillScreen(TFT_WHITE);
  for (uint8_t y = 0; y < qrcode.size; y++) {
    for (uint8_t x = 0; x < qrcode.size; x++) {
      uint16_t color = qrcode_getModule(&qrcode, x, y) ? TFT_BLACK : TFT_WHITE;
      tft.fillRect(offsetX + x * scale, offsetY + y * scale, scale, scale, color);
    }
  }

  free(buffer);
}

void displayError(String message) {
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(10, 10);
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.setTextSize(2);
  tft.println(message);
}
