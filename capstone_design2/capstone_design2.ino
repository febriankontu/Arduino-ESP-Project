#include <SPFD5408_Adafruit_GFX.h>
#include <SPFD5408_Adafruit_TFTLCD.h>
#include <SPFD5408_TouchScreen.h>

// ==== TFT LCD ====
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

// ==== Touchscreen ====
#define YP A1
#define XM A2
#define YM 7
#define XP 6
#define TS_MINX 100
#define TS_MAXX 920
#define TS_MINY 120
#define TS_MAXY 940
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

#define MINPRESSURE 10
#define MAXPRESSURE 1000

#define BLACK   0x0000
#define WHITE   0xFFFF
#define GREEN   0x07E0
#define RED     0xF800
#define YELLOW  0xFFE0
#define CYAN    0x07FF

#define BUTTON_X 60
#define BUTTON_Y 220
#define BUTTON_W 120
#define BUTTON_H 30

unsigned long totalHarga = 0;
int lineY = 20;
bool selesai = false;

void setup() {
  Serial.begin(9600);   // Debug
  Serial1.begin(9600);  // Komunikasi dengan Arduino 1
  tft.reset();
  tft.begin(0x9341);
  tft.setRotation(2);

  resetLcd();
}

void resetLcd() {
  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(20, 0);
  tft.println("DAFTAR BELANJA:");

  // Tombol SELESAI
  tft.fillRect(BUTTON_X, BUTTON_Y, BUTTON_W, BUTTON_H, CYAN);
  tft.setTextColor(BLACK);
  tft.setTextSize(2);
  tft.setCursor(BUTTON_X + 10, BUTTON_Y + 8);
  tft.println("SELESAI");

  // Reset variabel
  totalHarga = 0;
  lineY = 20;
  selesai = false;
}

bool tombolDitekan() {
  TSPoint p = ts.getPoint();
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    int x = map(p.x, TS_MINX, TS_MAXX, 0, 240);
    int y = map(p.y, TS_MINY, TS_MAXY, 0, 320);
    return (x >= BUTTON_X && x <= (BUTTON_X + BUTTON_W) &&
            y >= BUTTON_Y && y <= (BUTTON_Y + BUTTON_H));
  }
  return false;
}

void loop() {
  // Terima data dari Arduino 1
  if (Serial1.available()) {
    String data = Serial1.readStringUntil('\n');
    data.trim();

    if (data == "RESET") {
      // Reset tampilan dan variabel
      resetLcd();
    } 
    else if (data == "NOT_FOUND") {
      tft.setTextColor(RED);
      tft.setTextSize(1);
      tft.setCursor(10, lineY);
      tft.println("Barang Tidak Ada");
      lineY += 15;
    } 
    else {
      // Data produk: nama,harga
      int idx = data.indexOf(',');
      String nama = data.substring(0, idx);
      int harga = data.substring(idx + 1).toInt();

      // Tampilkan di LCD
      tft.setTextColor(GREEN);
      tft.setTextSize(1);
      tft.setCursor(10, lineY);
      tft.print(nama);
      tft.print(" Rp");
      tft.println(harga);
      lineY += 15;

      totalHarga += harga;
    }
  }

  // Tombol SELESAI ditekan
  if (!selesai && tombolDitekan()) {
    selesai = true;
    tft.setTextColor(YELLOW);
    tft.setTextSize(2);
    tft.setCursor(10, lineY + 10);
    tft.print("TOTAL: Rp ");
    tft.println(totalHarga);

    // Kirim perintah ke Arduino 1 untuk cetak struk
    Serial1.println("SELESAI");
  }
}
