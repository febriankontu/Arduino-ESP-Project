#include "Adafruit_Thermal.h"

struct Produk {
  const char* barcode;
  const char* nama;
  int harga;
};

const int MAX_PRODUK = 50;
Produk daftarProduk[MAX_PRODUK] = {
  {"8997212800295", "Cokelat Batang", 8000},
  {"8997212800301", "Permen Mint", 3000},
  {"8997212800318", "Roko", 36000},
  {"8997212800325", "Roti Tawar", 12000},
  {"8997212800332", "Mainan", 2000},
  {"8997212800349", "Plastik", 3000},
  {"8997212800356", "Keripik Kentang", 10000},
  {"8997212800363", "Air Mineral", 4000},
  {"8997212800370", "Lem Korea", 9000},
  {"8997212800387", "Callusol", 8000},
  {"8997212800493", "Mie Instan", 3000},
  {"8997212800509", "Sabun", 9000},
  {"8997212800516", "Skin Care", 120000},
  {"8997212800523", "Lem Korea", 9000},
  {"8997212800530", "Tisu Basah", 5000},
  {"8997212800547", "Roti", 5000},
  {"8997212800554", "Kopi", 12000},
  {"8997212800561", "Kecap Manis", 10000},
  {"8997212800578", "Gula Pasir", 14000},
  {"8997212800585", "Parfum", 160000},
  {"8997212800288", "Permen Buah", 3000},
  {"8994016000135", "Cotton Bud", 5000},
  {"8993176110074", "Minyak Kayu Putih", 8000},
  {"8998866601064", "Sabun Mandi", 7000},
  {"8996006855145", "Minuman Botol", 6000},
  {"8992775002582", "Snack", 5000},
  {"8996006852045", "Aqua", 4000},
  {"8996001600269", "Le Minerale", 3000},
  {"8998989110167", "Rokok", 36500},
  {"8990007910162", "Lem Korea", 9000},
  {"8991002101265", "Kopi", 7000},
  {"8998899995953", "Autan", 12000},
  {"8992745120650", "Obat Nyamuk", 20000}
};

Produk* keranjang[20];
int jumlahKeranjang = 0;
unsigned long totalHarga = 0;
bool selesai = false;

Adafruit_Thermal printer(&Serial2);

void setup() {
  Serial.begin(9600);     // Debugging
  Serial1.begin(9600);    // Barcode scanner
  Serial2.begin(9600);    // Thermal printer
  Serial3.begin(9600);    // Komunikasi ke Arduino 2 (TFT LCD)
}

Produk* cariProduk(const String& barcode) {
  for (int i = 0; i < MAX_PRODUK; i++) {
    if (barcode == daftarProduk[i].barcode) {
      return &daftarProduk[i];
    }
  }
  return nullptr;
}

void cetakStruk() {
  printer.begin();
  printer.setSize('M');
  printer.justify('C');
  printer.println("STRUK BELANJA");
  printer.println("------------------------");
  printer.justify('L');

  for (int i = 0; i < jumlahKeranjang; i++) {
    printer.print(i + 1);
    printer.print(". ");
    printer.print(keranjang[i]->nama);
    printer.print(" - Rp");
    printer.println(keranjang[i]->harga);
  }

  printer.println("------------------------");
  printer.setSize('L');
  printer.print("TOTAL: Rp ");
  printer.println(totalHarga);
  printer.feed(2);
  printer.sleep();
}

void loop() {
  // Baca barcode scanner
  if (Serial1.available()) {
    String data = Serial1.readStringUntil('\n');
    data.trim();

    Produk* produk = cariProduk(data);

    if (produk != nullptr && jumlahKeranjang < 20) {
      keranjang[jumlahKeranjang++] = produk;
      totalHarga += produk->harga;

      // Kirim ke Arduino 2
      Serial3.print(produk->nama);
      Serial3.print(",");
      Serial3.println(produk->harga);

    } else {
      // Barang tidak ditemukan
      Serial3.println("NOT_FOUND");
    }
  }

  // Tunggu perintah SELESAI dari Arduino 2
  if (!selesai && Serial3.available()) {
    String cmd = Serial3.readStringUntil('\n');
    cmd.trim();

    if (cmd == "SELESAI") {
      selesai = true;

      // Cetak struk
      cetakStruk();

      // Tunggu 5 detik supaya LCD TFT sempat menampilkan total
      delay(5000);

      // Kirim perintah RESET ke Arduino 2
      Serial3.println("RESET");

      // Reset variabel untuk pembelian berikutnya
      jumlahKeranjang = 0;
      totalHarga = 0;
      selesai = false;
    }
  }
}
