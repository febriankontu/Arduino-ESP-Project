/**************************CrowPanel ESP32 HMI Display Example Code************************
Version     :	1.1
Suitable for:	CrowPanel ESP32 HMI Display
Product link:	https://www.elecrow.com/esp32-display-series-hmi-touch-screen.html
Code	  link:	https://github.com/Elecrow-RD/CrowPanel-ESP32-Display-Course-File
Lesson	link:	https://www.youtube.com/watch?v=WHfPH-Kr9XU
Description	:	The code is currently available based on the course on YouTube, 
				        if you have any questions, please refer to the course video: Introduction 
				        to ask questions or feedback.
**************************************************************/

#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <SPI.h>
#include <qrcode_gen.h>
#include <TFT_eSPI.h>
#include <ArduinoJson.h>

/*******************************************************************************
   Config the display driver and touch driver in User_Setup.h(TFT_eSPI library)
 ******************************************************************************/

const char* ssid = "Arthatronic";
const char* password = "animatronics";

String apiKey = "eG5kX3Byb2R1Y3Rpb25fd21RWVhhRXREeWZwS0tpT29VaEowMzluS0dBYWUwdmxOV3Fja0tkZVN6eWJ6Smw3UnJpYVdocHpwOTBleWRR";
const String referenceId = "test_id_123";

#define BUTTON_PIN 18

TFT_eSPI tft = TFT_eSPI(); 

int btnW = 160;
int btnH = 80;
int btnY = 120;   // Auto center horizontal
int btnX = 160;  // Auto center vertical

bool isSleep = false;
unsigned long lastInteraction = 0;
bool touched = false;
unsigned long lastCheck = 0;
bool waitingForPayment = false;

void setup()
{
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // Button input

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  //Display Prepare
  tft.begin();
  pinMode(27, OUTPUT);
  digitalWrite(27, HIGH);
  tft.setRotation(1); 
  
  // showStartScreen();

  String qrText = getQRCode();
  if (!qrText.isEmpty()) {
    Serial.print("beriku adalah QR code: ");
    Serial.println(qrText);
    displayQRCode(qrText);
  } else {
    displayError("Failed to get QR");
  }
}

void loop() {
  uint16_t x, y;

  // === MODE SLEEP ===
  if (isSleep) {   
    if (tft.getTouch(&x, &y)) {
      digitalWrite(27, HIGH); // Nyalakan backlight
      // showStartScreen();      // Tampilkan tombol START
      isSleep = false;
      lastInteraction = millis(); // Reset timer
    }
  } else {
    // Auto sleep jika 60 detik tidak disentuh
    if (millis() - lastInteraction > 60000) {
      digitalWrite(27, LOW); // Matikan backlight
      isSleep = true;
    }

    // Cek sentuhan dan reset timer jika disentuh
    if (tft.getTouch(&x, &y)) {
      lastInteraction = millis();
    }

    // === CEK TOMBOL START ===
    // if (!touched) {
    //   if (tft.getTouch(&x, &y)) {
    //     y = tft.height() - y;  // Koreksi orientasi Y

    //     if (x > btnX && x < btnX + btnW && y > btnY && y < btnY + btnH) {
    //       touched = true;
    //       tft.fillScreen(TFT_BLACK);
    //       tft.setCursor(10, 10);
    //       tft.setTextColor(TFT_WHITE);
    //       tft.setTextSize(2);
    //       tft.println("Loading QR...");

    //       String qrText = getQRCode();
    //       if (!qrText.isEmpty()) {
    //         displayQRCode(qrText);
    //       } else {
    //         displayError("Failed to get QR");
    //         touched = false;
    //       }
    //     }
    //   }
    // } 
    // === CEK STATUS PEMBAYARAN ===
    // else {
      static unsigned long lastCheck = 0;

      if (millis() - lastCheck > 5000) {
        lastCheck = millis();

        if (checkPaymentStatus()) {
          Serial.println("Pembayaran berhasil!");

          touched = false;  // Reset agar tombol START bisa dipakai lagi
          tft.fillScreen(TFT_WHITE);  // atau tampilkan notifikasi sukses

          // Refresh QR
          String qrText = getQRCode();
          if (!qrText.isEmpty()) {
            displayQRCode(qrText);
          } else {
            displayError("Failed to get QR");
          }

        } 
        // else {
        //   Serial.println("Belum dibayar...");
        }
      }
  //   }
  // }

  // delay(100);
}


void showStartScreen() {
  tft.fillScreen(TFT_WHITE);
  // tft.setTextColor(TFT_BLACK, TFT_WHITE);
  // tft.setTextSize(2);
  // tft.setCursor(190, 50);
  // tft.println("Tap to generate QR");

  // Draw "START" button
  // int btnX = 120, btnY = 100, btnW = 200, btnH = 80;
  tft.fillRect(btnX, btnY, btnW, btnH, TFT_GREEN);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(btnX + 50, btnY + 30);
  tft.setTextSize(2);
  tft.println("START");
}



String getQRCode() {
  HTTPClient http;
  String url = "https://mountoya.arthatronic.com/api/rs/refill/xendit";
  http.begin(url);

  // Tambahkan header sesuai dengan kebutuhan API
  http.addHeader("Content-Type", "application/json");

  // Jika API memerlukan Authorization, tambahkan di sini
  // http.addHeader("Authorization", "Bearer YOUR_TOKEN");

  // Body JSON
  String payload = "{\"machine_id\": \"RS-001\", \"volume\": 500, \"type\": \"normal\", \"redirect_url\": \"index.html\"}";
  int httpResponseCode = http.POST(payload);

  Serial.print("HTTP Response Code: ");
  Serial.println(httpResponseCode);

  String response = http.getString();
  Serial.println("Response:");
  Serial.println(response);

  if (httpResponseCode == 200) {
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, response);

    if (error) {
      Serial.print("JSON parsing failed: ");
      Serial.println(error.c_str());
      http.end();
      return "";
    }

    // Ambil data.qr_string
    String qrText = doc["data"]["qr_string"] | "";
    http.end();
    return qrText;

  } else {
    Serial.println("HTTP request failed or unexpected response.");
    http.end();
    return "";
  }
}


void displayQRCode(String text) {
  const uint8_t QR_VERSION = 30;  // Cukup besar untuk string panjang dari Xendit
  int bufferSize = qrcode_getBufferSize(QR_VERSION);
  uint8_t* qrcodeData = (uint8_t*)malloc(bufferSize);

  if (!qrcodeData) {
    displayError("QR malloc failed");
    return;
  }

  QRCode qrcode;
  qrcode_initText(&qrcode, qrcodeData, QR_VERSION, ECC_LOW, text.c_str());

  int qrSize = qrcode.size;
  int scale = 2;  // kecilkan jika layar kecil
  int offsetX = (tft.width() - qrSize * scale) / 2;
  int offsetY = (tft.height() - qrSize * scale) / 2;

  tft.fillScreen(tft.color565(200, 200, 200));
  for (uint8_t y = 0; y < qrSize; y++) {
    for (uint8_t x = 0; x < qrSize; x++) {
      uint16_t color = qrcode_getModule(&qrcode, x, y) ? TFT_BLACK : TFT_WHITE;
      tft.fillRect(offsetX + x * scale, offsetY + y * scale, scale, scale, color);
    }
  }

  free(qrcodeData);
}

void displayError(String message) {
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0);
  tft.println(message);
}

bool checkPaymentStatus() {
  HTTPClient http;
  String url = "https://mountoya.arthatronic.com/api/rs/refill/status";
  http.begin(url);

  http.addHeader("Content-Type", "application/json");

  // Body JSON, gunakan reference ID unik untuk transaksi QR yang aktif
  String payload = "{\"reference_id\": \"" + referenceId + "\"}";
  int httpResponseCode = http.POST(payload);

  Serial.print("Check status response: ");
  Serial.println(httpResponseCode);

  if (httpResponseCode == 200) {
    String response = http.getString();
    Serial.println(response);

    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, response);
    if (error) {
      Serial.print("JSON Error: ");
      Serial.println(error.c_str());
      http.end();
      return false;
    }

    String status = doc["data"]["status"] | "";
    http.end();

    return status == "PAID";  // ganti sesuai format API kamu
  }

  http.end();
  return false;
}
