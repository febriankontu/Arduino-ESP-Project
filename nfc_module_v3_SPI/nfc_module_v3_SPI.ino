#include <SPI.h>
#include <Adafruit_PN532.h>

// Definisi pin SPI (sesuai koneksi)
#define PN532_SCK  18
#define PN532_MOSI 23
#define PN532_MISO 19
#define PN532_SS   5   // Chip select

// Inisialisasi PN532 dengan SPI
Adafruit_PN532 nfc(PN532_SS, &SPI);

void setup(void) {
  Serial.begin(115200);
  Serial.println("Inisialisasi NFC (PN532) via SPI...");

  // Mulai SPI (gunakan default pin atau custom jika perlu)
  SPI.begin(PN532_SCK, PN532_MISO, PN532_MOSI);

  // Inisialisasi NFC
  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println("Tidak ada modul NFC terdeteksi.");
    while (1); // Berhenti
  }

  Serial.print("Ditemukan PN5"); Serial.println((versiondata >> 24) & 0xFF, HEX);
  Serial.print("Firmware Version: "); Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata >> 8) & 0xFF, DEC);

  nfc.SAMConfig(); // Set PN532 sebagai reader
  Serial.println("Menunggu kartu NFC...");
}

void loop(void) {
  uint8_t uid[7];
  uint8_t uidLength;

  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
    Serial.print("Kartu terdeteksi! UID: ");
    for (uint8_t i = 0; i < uidLength; i++) {
      Serial.print(uid[i], HEX);
      if (i < uidLength - 1) Serial.print(":");
    }
    Serial.println();
    delay(1000);
  }
}
