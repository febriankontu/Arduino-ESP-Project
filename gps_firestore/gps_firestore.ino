#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <TinyGPSPlus.h>

// Addons untuk Token dan Firestore
#include "addons/TokenHelper.h"

/* 1. TENTUKAN KREDENSIAL WIFI & FIREBASE */
#define WIFI_SSID "HINT.DOO"
#define WIFI_PASSWORD "riyankres5"

// API Key sama seperti sebelumnya
#define API_KEY "AIzaSyAzSGDRNwlLvcSCPomMYcfihT5pNdaYXKA"

// PENTING: Untuk Firestore, kita butuh PROJECT ID.
// Diambil dari bagian depan URL database kamu: "npkmonitoring-b0d7c"
#define FIREBASE_PROJECT_ID "npkmonitoring-b0d7c"

/* 2. OBJEK & VARIABEL */
TinyGPSPlus gps;
HardwareSerial gpsSerial(2); // Pastikan pin RX/TX sesuai rangkaianmu

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;

void setup() {
  Serial.begin(115200);
  // Sesuaikan pin RX, TX (contoh: RX=16, TX=17)
  gpsSerial.begin(9600, SERIAL_8N1, 16, 17);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());

  /* 3. KONFIGURASI FIREBASE */
  config.api_key = API_KEY;

  // Di Firestore, Database URL tidak wajib, tapi Project ID wajib!
  config.service_account.data.project_id = FIREBASE_PROJECT_ID;

  // Sign up anonim
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase SignUp OK");
    signupOK = true;
  } else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  // 4. PERBAIKAN LOGIKA GPS (PENTING!)
  // gps.encode() HARUS dijalankan terus menerus setiap milidetik
  // agar data satelit terbaca. Jangan dimasukkan ke dalam delay/timer.
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }

  // 5. KIRIM DATA SETIAP 5 DETIK
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 5000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();

    // Cek apakah GPS sudah punya lokasi valid
    if (gps.location.isValid()) {
      float lat = gps.location.lat();
      float lng = gps.location.lng();

      Serial.print("Lat: "); Serial.println(lat, 6);
      Serial.print("Lng: "); Serial.println(lng, 6);

      // --- MENYUSUN DATA UNTUK FIRESTORE ---
      // Firestore butuh format JSON khusus.
      // Kita gunakan objek FirebaseJson.
      FirebaseJson content;

      // "doubleValue" memberi tahu Firestore bahwa ini adalah angka (bukan string)
      // Structure: fields -> latitude -> doubleValue: -6.9
      content.set("fields/latitude/doubleValue", lat);
      content.set("fields/longitude/doubleValue", lng);

      // --- MENGIRIM KE FIRESTORE ---
      // Path: GPS (collection) -> live_data (document ID)
      // patchDocument: update data yang ada, atau buat baru jika belum ada
      Serial.print("Mengirim ke Firestore... ");
      
      if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "" /* databaseId default */, "GPS/live_data", content.raw(), "latitude,longitude")) {
        Serial.println("BERHASIL!");
      } else {
        Serial.print("GAGAL: ");
        Serial.println(fbdo.errorReason());
      }
    } else {
      Serial.println("Menunggu sinyal GPS (Cari tempat terbuka)...");
    }
  }
}