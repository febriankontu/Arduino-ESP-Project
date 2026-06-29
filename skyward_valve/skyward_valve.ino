#include <Wire.h>
#include <Adafruit_PN532.h>

#define CLK_PIN 5             // First rotary encoder CLK (A)
#define DT_PIN  1             // First rotary encoder DT (B)
#define CLK1_PIN 22            // Second rotary encoder CLK (A)
#define DT1_PIN  4            // Second rotary encoder DT (B)

// RFID Pin definitions for ESP32-C6 I2C
#define SDA_PIN 6
#define SCL_PIN 7

#define RELAY1_PIN 23  // Change this pin if needed
#define RELAY2_PIN 10 
#define RELAY3_PIN 11 

// Create PN532 instance
Adafruit_PN532 nfc(SDA_PIN, SCL_PIN);

volatile int encoderCount = 0;
volatile int encoderCount1 = 0;
volatile int direction1 = 0; // -1 = CCW, 1 = CW, 0 = None
volatile int direction2 = 0;

volatile int pulseBuffer1 = 0;
volatile int pulseBuffer2 = 0;

volatile unsigned long lastMoveTime1 = 0;
volatile unsigned long lastMoveTime2 = 0;
volatile unsigned long lastMoveTime3 = 0;

// Variables for RFID tag detection
bool tagPresent = false;
uint8_t lastUID[7];
uint8_t lastUIDLength = 0;
uint64_t currentUIDDecimal = 0;

String serialInput = "";

// Variables for LED blinking
bool blinkingRed = false;
bool blinkingGreen = false;
bool blinkingBlue = false;
bool blinkingWhite = false;
unsigned long lastBlinkTime = 0;
bool blinkState = false;
const unsigned long BLINK_INTERVAL = 500; // 500ms blink interval

// --- KONFIGURASI PIN LAMPU RGB (UNTUK ESP32-C6) ---
// Lampu RGB
const int RGB_RED_PIN = 12;    // PIN untuk ESP32-C6
const int RGB_BLUE_PIN = 13;   // PIN untuk ESP32-C6
const int RGB_GREEN_PIN = 18; // PIN untuk ESP32-C6

// Konfigurasi PWM (LEDC) untuk ESP32
const int PWM_FREQ = 5000; // Frekuensi 5KHz
const int PWM_RESOLUTION = 8; // Resolusi 8-bit (0-255)

// Function declarations
void setRGB(int r, int g, int b);
void handleBlinking();
void stopAllBlinking();

void IRAM_ATTR readEncoder() {
  int dtValue = digitalRead(DT_PIN);
  int movement = (dtValue == HIGH) ? -1 : 1;
  encoderCount += movement;
  pulseBuffer1 += movement;
  lastMoveTime1 = millis();

  if (abs(pulseBuffer1) > 180) {
    direction1 = (pulseBuffer1 > 0) ? 1 : -1;
    pulseBuffer1 = 0; // Reset after confirming direction
  }
}

void IRAM_ATTR readEncoder1() {
  int dtValue = digitalRead(DT1_PIN);
  int movement = (dtValue == HIGH) ? -1 : 1;
  encoderCount1 += movement;
  pulseBuffer2 += movement;
  lastMoveTime2 = millis();

  if (abs(pulseBuffer2) > 180) {
    direction2 = (pulseBuffer2 > 0) ? 1 : -1;
    pulseBuffer2 = 0;
  }
}

String getDirectionName(int dir) {
  
    if (dir == 1) return "1";
    else if (dir == -1) return "-1";  
    else return "0";
  
}

// Function to read RFID and return UID decimal
bool checkRFID(uint64_t* uid_decimal) {
  uint8_t success;
  uint8_t uid[7]; // Buffer to store the returned UID
  uint8_t uidLength; // Length of the UID
  
  // Wait for an ISO14443A type card (Mifare, etc.)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 50);
  
  if (success) {
    // Check if this is a new tag or the same tag
    bool isNewTag = false;
    if (uidLength != lastUIDLength) {
      isNewTag = true;
    } else {
      for (uint8_t i = 0; i < uidLength; i++) {
        if (uid[i] != lastUID[i]) {
          isNewTag = true;
          break;
        }
      }
    }
    
    // Only process if it's a new tag or tag was removed and placed again
    if (isNewTag || !tagPresent) {
      // Convert UID to decimal (reversed for encryption compatibility)
      *uid_decimal = 0;
      for (int8_t i = uidLength - 1; i >= 0; i--) {
        *uid_decimal = (*uid_decimal << 8) | uid[i];
      }
      
      // Store current UID  
      lastUIDLength = uidLength;
      for (uint8_t i = 0; i < uidLength; i++) {
        lastUID[i] = uid[i];
      }
      
      tagPresent = true;
      currentUIDDecimal = *uid_decimal;
      return true; // New tag detected
    }
    
    tagPresent = true;
    *uid_decimal = currentUIDDecimal; // Return current UID decimal
    return false; // Same tag, no new detection
  } else {
    // No tag present
    if (tagPresent) {
      tagPresent = false;
      currentUIDDecimal = 0;
    }
    *uid_decimal = 0;
    return false;
  }
}

void setup() {
  Serial.begin(115200);

  // Rotary encoder 1
  pinMode(CLK_PIN, INPUT_PULLUP);
  pinMode(DT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(CLK_PIN), readEncoder, RISING);

  // Rotary encoder 2
  pinMode(CLK1_PIN, INPUT_PULLUP);
  pinMode(DT1_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(CLK1_PIN), readEncoder1, RISING);

  // Relay setup
  pinMode(RELAY1_PIN, OUTPUT);
  digitalWrite(RELAY1_PIN, LOW);  // Relay OFF by default
  pinMode(RELAY2_PIN, OUTPUT);
  digitalWrite(RELAY2_PIN, LOW);
  pinMode(RELAY3_PIN, OUTPUT);
  digitalWrite(RELAY3_PIN, LOW);

  // --- RFID INITIALIZATION ---
  Serial.println("Initializing RFID I2C and PN532...");
  
  // Initialize I2C with explicit pins and frequency for ESP32-C6
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(100000); // Set I2C frequency to 100kHz (slower for stability)
  
  delay(500); // Give time for I2C to stabilize
  
  // Initialize NFC module
  nfc.begin();
  delay(100);
  
  // Try to connect to PN532
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println("Warning: PN532 not found, continuing without RFID...");
  } else {
    Serial.print("Found RFID chip PN5"); 
    Serial.println((versiondata>>24) & 0xFF, HEX);
    if (!nfc.SAMConfig()) {
      Serial.println("Warning: SAM configuration failed, but continuing...");
    }
    Serial.println("-> RFID PN532 initialized");
  }

  // Inisialisasi Lampu RGB menggunakan LEDC (PWM) - API baru
  // Menggunakan ledcAttach dengan frekuensi dan resolusi
  ledcAttach(RGB_RED_PIN, PWM_FREQ, PWM_RESOLUTION);
  ledcAttach(RGB_GREEN_PIN, PWM_FREQ, PWM_RESOLUTION);
  ledcAttach(RGB_BLUE_PIN, PWM_FREQ, PWM_RESOLUTION);
  
  // Set semua LED ke OFF (255 = OFF untuk hardware ini)
  ledcWrite(RGB_RED_PIN, 255);
  ledcWrite(RGB_GREEN_PIN, 255);
  ledcWrite(RGB_BLUE_PIN, 255);
  
  Serial.println("-> RGB LED terinisialisasi [OFF]");
  
  Serial.println("\n--- DAFTAR PERINTAH RGB ---");
  Serial.println("LED Preset:");
  Serial.println("  red1, green1, blue1, white1 - Warna solid");
  Serial.println("LED Blinking:");
  Serial.println("  red1_blink, green1_blink, blue1_blink, white1_blink - Warna berkedip");
  Serial.println("  off - Matikan LED");
  Serial.println("--- DAFTAR PERINTAH RELAY ---");
  Serial.println("  a - Relay 1 ON 2 detik");
  Serial.println("  b - Relay 2 ON 2 detik");
  Serial.println("  c - Relay 3 ON 2 detik");
  Serial.println("  successful - Relay 1 ON 2 detik");
  Serial.println("-------------------------");

  Serial.println("Dual Rotary Encoder System with RFID and RGB Initialized");
}

void loop() {
  static int lastCount = 0;
  static int lastCount1 = 0;

  // Handle LED blinking
  handleBlinking();

  // === Handle Serial Input ===
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      serialInput.trim();
      
      // RGB Commands
      if (serialInput == "red1") {
        stopAllBlinking();
        setRGB(0, 255, 255); // Red ON, Green OFF, Blue OFF
      } else if (serialInput == "green1") {
        stopAllBlinking();
        setRGB(255, 0, 255); // Red OFF, Green ON, Blue OFF
      } else if (serialInput == "blue1") {
        stopAllBlinking();
        setRGB(255, 255, 0); // Red OFF, Green OFF, Blue ON
      } else if (serialInput == "white1") {
        stopAllBlinking();
        setRGB(0, 0, 0); // Red ON, Green ON, Blue ON
      } else if (serialInput == "yellow1") {
        stopAllBlinking();
        setRGB(0, 150, 255); // Red ON, Green ON, Blue ON
      } else if (serialInput == "red1_blink") {
        stopAllBlinking();
        blinkingRed = true;
      } else if (serialInput == "green1_blink") {
        stopAllBlinking();
        blinkingGreen = true;
      } else if (serialInput == "blue1_blink") {
        stopAllBlinking();
        blinkingBlue = true;
      } else if (serialInput == "white1_blink") {
        stopAllBlinking();
        blinkingWhite = true;
      } else if (serialInput == "off") {
        stopAllBlinking();
        setRGB(255, 255, 255); // All OFF
      }
      // Original Relay Commands
      else if (serialInput == "a") {
        digitalWrite(RELAY1_PIN, HIGH);
        delay(2000);
        digitalWrite(RELAY1_PIN, LOW);
      } else if (serialInput == "b") {
        digitalWrite(RELAY2_PIN, HIGH);
        delay(2000);
        digitalWrite(RELAY2_PIN, LOW);
      } else if (serialInput == "c") {
        digitalWrite(RELAY3_PIN, HIGH);
        delay(2000);
        digitalWrite(RELAY3_PIN, LOW);
      } else if (serialInput == "successful") {
        Serial.println("Command: successful — Activating Relay");
        digitalWrite(RELAY1_PIN, HIGH);
        delay(2000);
        digitalWrite(RELAY1_PIN, LOW);
      }
      serialInput = "";
    } else {
      serialInput += c;
    }
  }

  // === Wrap encoder counts ===
  if (encoderCount >= 360 || encoderCount <= -360) {
    encoderCount = 0;
  }
  if (encoderCount1 >= 360 || encoderCount1 <= -360) {
    encoderCount1 = 0;
  }

  // === Check RFID ===
  uint64_t uid_decimal = 0;
  checkRFID(&uid_decimal);

  // === Display encoder values and RFID ===
  // Check if direction should timeout
  if (millis() - lastMoveTime1 > 300) {
    direction1 = 0;
    pulseBuffer1 = 0;
  }
  if (millis() - lastMoveTime2 > 300) {
    direction2 = 0;
    pulseBuffer2 = 0;
  }

  // if(millis() - lastMoveTime3 > 500){
  //   lastMoveTime3 = millis();
    Serial.print(getDirectionName(direction1));
    Serial.print(",");
    Serial.print(getDirectionName(direction2));
    Serial.print(",");
    Serial.print(uid_decimal);
    Serial.println(",");

    // Serial.print(encoderCount);
    // Serial.print(",");
    // Serial.println(encoderCount1);  
    
    lastCount = encoderCount;
    lastCount1 = encoderCount1;
  // }

  delay(100); // Reduced delay for smoother blinking
}

// --- FUNGSI BANTUAN ---

// Fungsi untuk handle blinking
void handleBlinking() {
  unsigned long currentTime = millis();
  
  if (currentTime - lastBlinkTime >= BLINK_INTERVAL) {
    blinkState = !blinkState;
    lastBlinkTime = currentTime;
    
    // Handle red blinking
    if (blinkingRed) {
      if (blinkState) {
        setRGB(0, 255, 255); // Red ON
      } else {
        setRGB(255, 255, 255); // All OFF
      }
    }
    
    // Handle green blinking
    if (blinkingGreen) {
      if (blinkState) {
        setRGB(255, 0, 255); // Green ON
      } else {
        setRGB(255, 255, 255); // All OFF
      }
    }
    
    // Handle blue blinking
    if (blinkingBlue) {
      if (blinkState) {
        setRGB(255, 255, 0); // Blue ON
      } else {
        setRGB(255, 255, 255); // All OFF
      }
    }
    
    // Handle white blinking
    if (blinkingWhite) {
      if (blinkState) {
        setRGB(0, 0, 0); // White ON (all colors)
      } else {
        setRGB(255, 255, 255); // All OFF
      }
    }
  }
}

// Fungsi untuk menghentikan semua blinking
void stopAllBlinking() {
  blinkingRed = false;
  blinkingGreen = false;
  blinkingBlue = false;
  blinkingWhite = false;
}

// Fungsi untuk set RGB dengan nilai langsung
void setRGB(int r, int g, int b) {
  // Hardware menggunakan logika terbalik: 255 = OFF, 0 = ON
  // Input parameter menggunakan logika hardware langsung
  r = constrain(r, 0, 255);
  g = constrain(g, 0, 255);
  b = constrain(b, 0, 255);
  
  ledcWrite(RGB_RED_PIN, r);
  ledcWrite(RGB_GREEN_PIN, g);
  ledcWrite(RGB_BLUE_PIN, b);
}