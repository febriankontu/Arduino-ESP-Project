// Modified for SPFD5408 Library by Joao Lopes

#include "SPFD5408_Adafruit_GFX.h"    // Core graphics library
#include "SPFD5408_Adafruit_TFTLCD.h" // Hardware-specific library
#include "SPFD5408_TouchScreen.h"

// Pin assignments for the TFT touch screen
#define YP    A1
#define XM    A2
#define YM    7
#define XP    6

// Calibrated values for the TFT touch screen  
#define TS_MINX   178
#define TS_MINY   75
#define TS_MAXX   931
#define TS_MAXY   895

// Instantiate the ts object  
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

#define LCD_RD    A0
#define LCD_WR    A1
#define LCD_CD    A2
#define LCD_CS    A3
#define LCD_RESET A4

// Instantiate the tft object
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

void setup(void) {
  Serial.begin(9600);
  Serial.println(F("Paint!"));
  
  tft.reset();
  tft.begin(0x9341);      // SPFD5408 compatible
  tft.setRotation(2);     // This settings works for me with correct orientation
                          // tft.setRotation(2);
  tft.fillScreen(BLACK);  //Set Background Color with BLACK

  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.setCursor(10, 10);
  tft.println("Hello from TechToTinker");

  tft.setTextColor(RED);
  tft.setTextSize(1.5);
  tft.setCursor(10, 30);
  tft.println("2.4 TFT Display");
  delay(3000);
  tft.fillScreen(BLACK);
}

void loop() {
  //tft.fillScreen(ILI9341_BLACK); // Bersihkan layar

  const char* items[] = {"Permen", "Rokok", "Mie Instan", "Parfum", "Kunci", "Minyak", "Botol", "Plastik", "Cutter", "Gunting"};     // Daftar huruf yang ditampilkan
  float textSize = 2;
  int lineHeight = 20;  // Tinggi per baris (8px font + padding)
  int y = 10;                         // Posisi Y awal

  tft.setTextColor(WHITE);
  tft.setTextSize(textSize);

  int itemCount = sizeof(items) / sizeof(items[0]);

  for (int i = 0 ; i < itemCount ; i++) {
    tft.setCursor(10, y);            // Atur posisi otomatis
    tft.print(i + 1);                // Cetak nomor (1, 2, 3,...)
    tft.print(". ");
    tft.println(items[i]);           // Cetak huruf (A, B, C,...)

    y += lineHeight;                 // Geser ke bawah untuk baris berikutnya
    //delay(2000);                     // Tampilkan satu per satu
  }

  //delay(4000);  // Tunggu sebelum mengulang
}


// void loop() {

//   int y = 10;  // Posisi awal Y
//   int lineHeight = 10; // Jarak antar baris (bisa disesuaikan tergantung ukuran teks)

//   for (int i = 0; i < 3; i++) {
//     tft.setTextColor(WHITE);
//     tft.setTextSize(1);
//     tft.setCursor(10, y);  // Posisi otomatis

//     if (i == 0) {
//       tft.println("1. A");
//     } else if (i == 1) {
//       tft.println("2. B");
//     } else if (i == 2) {
//       tft.println("3. C");
//       i = 0;
//     }

//     y += lineHeight; // Geser ke bawah untuk baris berikutnya
//     delay(2000);
//   }

  //delay(4000); // Tunggu sebelum mengulang
//}


// void loop() {
//   for(int i = 0; i<3; i++){
//     if(i == 0){
//         tft.setTextColor(WHITE);
//         tft.setTextSize(1);
//         tft.setCursor(10, 10);
//         tft.println("1. A");
//     }
//     else if(i == 1){
//         tft.setTextColor(WHITE);
//         tft.setTextSize(1);
//         tft.setCursor(10, 20);
//         tft.println("2. B");
//     }
//     else if(i == 2){
//         tft.setTextColor(WHITE);
//         tft.setTextSize(1);
//         tft.setCursor(10, 30);
//         tft.println("3. C");
//     }
//     delay(2000);
//   }
// }
