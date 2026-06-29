#ifndef __QRCODE_H__
#define __QRCODE_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define QR_ECLEVEL_L 0
#define QR_ECLEVEL_M 1
#define QR_ECLEVEL_Q 2
#define QR_ECLEVEL_H 3

typedef struct {
    uint8_t version;
    uint8_t size;
    uint8_t data[];
} QRCode;

int qrcode_getBufferSize(uint8_t version);
void qrcode_initText(QRCode *qrcode, uint8_t *buffer, uint8_t version, uint8_t eclevel, const char *data);
uint8_t qrcode_getModule(QRCode *qrcode, uint8_t x, uint8_t y);

#ifdef __cplusplus
}
#endif

#endif
