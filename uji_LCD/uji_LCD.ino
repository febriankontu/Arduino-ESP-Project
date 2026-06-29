#include <SPFD5408_Adafruit_GFX.h>
#include <SPFD5408_Adafruit_TFTLCD.h>
#include <SPFD5408_TouchScreen.h>

// Pin TFT
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

// Inisialisasi TFT
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

// Warna RGB565
#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define ORANGE  0xFD20

void setup() {
  tft.reset();
  tft.begin(0x9341);       // ID SPFD5408 biasanya 0x9341
  tft.setRotation(1);      // Landscape
  tft.fillScreen(BLACK);

  // === Teks Ukuran & Warna ===
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 20);
  tft.println("Teks Ukuran 2");

  tft.setTextColor(RED);
  tft.setTextSize(3);
  tft.setCursor(10, 50);
  tft.println("Merah Besar");

  tft.setTextColor(YELLOW);
  tft.setTextSize(1);
  tft.setCursor(10, 90);
  tft.println("Teks Kecil Kuning");

  // === Menampilkan Angka ===
  int nilai = 12345;
  tft.setTextColor(CYAN);
  tft.setTextSize(2);
  tft.setCursor(10, 120);
  tft.print("Nilai: ");
  tft.println(nilai);

  // === Bentuk ===
  tft.fillRect(10, 150, 60, 30, BLUE);      // Kotak Biru
  tft.fillCircle(100, 165, 15, GREEN);      // Lingkaran Hijau
  tft.drawLine(150, 150, 220, 180, RED);    // Garis Merah
  tft.drawRoundRect(10, 190, 100, 40, 8, MAGENTA); // Kotak sudut bulat

  // Label bentuk
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.setCursor(120, 210);
  tft.println("Kotak, Lingkaran, Garis, Rounded");
}

void loop() {
  // Tidak perlu mengulang
}
