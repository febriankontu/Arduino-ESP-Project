struct Produk {
  const char* barcode;
  const char* nama;
};

const int MAX_PRODUK = 6;

Produk daftarProduk[MAX_PRODUK] = {
  {"8997212800295", "Fox BERRIES"},
  {"8997212800288", "Permen Kaleng"},
  {"8994016000135", "Cotton Bud"},
  {"8993176110074", "Minyak Kayu Putih"},
  {"8996001600269", "Aqua"}
};

const int jumlahProduk = sizeof(daftarProduk) / sizeof(daftarProduk[0]);

// Fungsi untuk mencari nama produk berdasarkan barcode
const char* cariProduk(const String& barcode) {
  for (int i = 0; i < jumlahProduk; i++) {
    if (barcode == daftarProduk[i].barcode) {
      return daftarProduk[i].nama;
    }
  }
  return "Barang Tidak Ada";
}

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600); 
}

void loop() {
  if (Serial1.available()) {
    String data = Serial1.readStringUntil('\n');
    // data.trim(); 
    const char* namaProduk = cariProduk(data);
    Serial.print(data);
    Serial.print(": ");
    Serial.println(namaProduk);
  }
}
