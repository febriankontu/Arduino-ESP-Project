#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xAD };

#define W5500_CS 5  // CS sesuai wiring
EthernetClient client;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Ethernet.init(W5500_CS);
  Serial.println("Inisialisasi Ethernet...");

  if (Ethernet.begin(mac) == 0) {
    Serial.println("Gagal mendapatkan IP dari DHCP");
    Serial.println("Coba ulang DHCP...");
    // Coba ulang beberapa kali
    for (int i = 0; i < 5; i++) {
      delay(2000);
      if (Ethernet.begin(mac) != 0) break;
    }

    if (Ethernet.localIP() == IPAddress(0, 0, 0, 0)) {
      Serial.println("Tetap gagal. Periksa kabel LAN dan router.");
      while (true);
    }
  }

  Serial.print("IP Address: ");
  Serial.println(Ethernet.localIP());
}

void loop() {
}
