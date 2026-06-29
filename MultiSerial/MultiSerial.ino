/*
  Multiple Serial test

  Receives from the main serial port, sends to the others.
  Receives from serial port 1, sends to the main serial (Serial 0).

  This example works only with boards with more than one serial like Arduino Mega, Due, Zero etc.

  The circuit:
  - any serial device attached to Serial port 1
  - Serial Monitor open on Serial port 0

  created 30 Dec 2008
  modified 20 May 2012
  by Tom Igoe & Jed Roach
  modified 27 Nov 2015
  by Arturo Guadalupi

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/MultiSerialMega
*/


#include <SPFD5408_Adafruit_GFX.h>
#include <SPFD5408_Adafruit_TFTLCD.h>

// Definisi pin untuk TFT LCD
#define LCD_CS   A3
#define LCD_CD   A2
#define LCD_WR   A1
#define LCD_RD   A0
#define LCD_RESET A4

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

// Warna
#define BLACK   0x0000
#define WHITE   0xFFFF
#define GREEN   0x07E0

int yPos = 0; // Posisi vertikal awal untuk tulisan

void setup() {
  Serial.begin(9600);      // Untuk Serial Monitor
  Serial1.begin(9600);     // Untuk GM65 atau device lain di Serial1

  // Inisialisasi LCD
  tft.reset();
  tft.begin(0x9341);       // ID untuk SPFD5408
  tft.setRotation(2);
  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  // tft.setCursor(10, yPos);
  // tft.println("Monitoring...");
  // yPos += 20;
}

void loop() {
  // Jika ada data masuk dari Serial1 (misalnya dari scanner)
  if (Serial1.available()) {
    String data = Serial1.readStringUntil('\n');
    data.trim();
    Serial.println(data); // Tampilkan di Serial Monitor

    // Tampilkan di LCD
    tft.setCursor(10, yPos);
    tft.setTextColor(GREEN);
    tft.println(data);
    yPos += 20;

    // Reset tampilan jika sudah melebihi batas layar
    if (yPos > 220) {
      tft.fillScreen(BLACK);
      yPos = 0;
    }
  }

  // Jika ada data diketik di Serial Monitor, kirim ke Serial1
  if (Serial.available()) {
    char ch = Serial.read();
    Serial1.write(ch);
  }
}
