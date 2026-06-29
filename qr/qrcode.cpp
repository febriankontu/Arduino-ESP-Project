#include "qrcode.h"
#include <cstring>

// Untuk ringkasan dan contoh demo saja.
// Silakan ganti dengan versi penuh dari: https://github.com/ricmoo/QRCode

int qrcode_getBufferSize(uint8_t version) {
  return 134;  // Cukup untuk QR versi kecil (v1–v3)
}

void qrcode_initText(QRCode *qrcode, uint8_t *buffer, uint8_t version, uint8_t eclevel, const char *data) {
  // Implementasi palsu (dummy)
  qrcode->version = version;
  qrcode->size = 21;
  for (int i = 0; i < 21 * 21; i++) {
    buffer[i] = (i % 2 == 0) ? 1 : 0;
  }
  memcpy(qrcode->data, buffer, 21 * 21);
}

uint8_t qrcode_getModule(QRCode *qrcode, uint8_t x, uint8_t y) {
  if (x >= qrcode->size || y >= qrcode->size) return 0;
  return qrcode->data[y * qrcode->size + x];
}
