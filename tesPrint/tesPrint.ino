#include "Adafruit_Thermal.h"

Adafruit_Thermal printer(&Serial2);  // Gunakan Serial1 (atau sesuaikan port Serial)

// ==== Daftar Produk ====
struct Produk {
  const char* nama;
  int harga;
};

Produk keranjang[] = {
  {"Fox BERRIES", 3000},
  {"Fox FRUITS", 3000},
  {"Cotton Bud", 5000},
  {"Minyak Kayu Putih", 8000},
  {"Sabun Mandi", 7000}
};
const int jumlahProduk = sizeof(keranjang) / sizeof(keranjang[0]);

void setup() {
  Serial.begin(9600);    // Debug (opsional)
  Serial2.begin(9600);   // Thermal printer

  printer.begin();
  printer.setDefault();  // Reset ke default

  // Cetak Struk
  cetakStruk();
}

void loop() {
  // Tidak ada yang dilakukan di loop
}

void cetakStruk() {
  int total = 0;

  printer.justify('C');
  printer.setSize('M');
  printer.println("STRUK BELANJA");
  printer.println("----------------------");
  printer.justify('L');

  for (int i = 0; i < jumlahProduk; i++) {
    printer.print(i + 1);
    printer.print(". ");
    printer.print(keranjang[i].nama);
    printer.print(" - Rp");
    printer.println(keranjang[i].harga);
    total += keranjang[i].harga;
  }

  printer.println("----------------------");
  printer.setSize('L');
  printer.print("TOTAL: Rp ");
  printer.println(total);

  printer.feed(2);
  printer.sleep();
}
