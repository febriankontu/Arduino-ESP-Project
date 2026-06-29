// Modified for SPFD5408 Library by Joao Lopes  
 #include <SPFD5408_Adafruit_GFX.h>  // Core graphics library  
 #include <SPFD5408_Adafruit_TFTLCD.h> // Hardware-specific library  
 #include <SPFD5408_TouchScreen.h>  
 // Pin assignments for the TFT touch screen  
 #define YP  A1  
 #define XM  A2  
 #define YM  7  
 #define XP  6  
 // Calibrated values for the TFT touch screen   
 #define TS_MINX  178  
 #define TS_MINY  75  
 #define TS_MAXX  931  
 #define TS_MAXY  895  
 // Instantiate the ts object  
 TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);  
 #define LCD_RD  A0  
 #define LCD_WR  A1  
 #define LCD_CD  A2  
 #define LCD_CS  A3  
 #define LCD_RESET A4  
 // Instantiate the tft object  
 Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);  
 // Assign human-readable names to some common 16-bit color values:  
 #define BLACK  0x0000  
 #define BLUE  0x001F  
 #define RED   0xF800  
 #define GREEN  0x07E0  
 #define CYAN  0x07FF  
 #define MAGENTA 0xF81F  
 #define YELLOW 0xFFE0  
 #define WHITE  0xFFFF  
 #define BOXSIZE   40  
 #define PENRADIUS  3  
 #define MINPRESSURE 10  
 #define MAXPRESSURE 1000  
 #define LED_PIN   A5  
 int touch = 0;  
 void setup() {  
  pinMode(LED_PIN, OUTPUT);  
  digitalWrite(LED_PIN, LOW);  
  Serial.begin(9600);  
  tft.reset();  
  tft.begin(0x9341);  
  tft.setRotation(2); // This settings works for me with correct orientation  
            // tft.setRotation(2);  
  tft.setTextColor(WHITE);  
  tft.setTextSize(2);  
  tft.fillScreen(BLACK);  
  tft.fillRect(0, 0, 120, 120, GREEN);  
  tft.fillRect(120, 0, 120, 120, RED);  
  tft.setCursor(15, 45);  
  tft.println("LED ON");  
  tft.setCursor(128, 45);  
  tft.println("LED OFF");  
  tft.setTextColor(WHITE, BLACK);  
  tft.setTextSize(8);  
 }  
 void loop() {  
  // Get the touch points  
  TSPoint p = ts.getPoint();  
  // Restore the mode  
  pinMode(XM, OUTPUT);  
  pinMode(YP, OUTPUT);  
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {  
   p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());  
   p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());  
   // Touch area for box 1  
   if (p.x > 0 && p.x < 120) {  
    if (p.y > 0 && p.y < 120) {  
     touch = 1;  
    }  
   }  
   // Touch area for box 2  
   if (p.x > 120 && p.x < 240) {  
    if (p.y > 0 && p.y < 120) {  
     touch = 2;  
    }  
   }  
  }  
  // Process the touch in box 1  
  if (touch == 1) {  
   digitalWrite(LED_PIN, HIGH);  
   tft.setCursor(100,200);  
   tft.print("1");  
   touch = 0;  
   delay(100);  
  }  
  // Process the touch in box 2  
  if (touch == 2) {  
   digitalWrite(LED_PIN, LOW);  
   tft.setCursor(100,200);  
   tft.print("0");  
   touch = 0;  
   delay(100);  
  }  
 }  
