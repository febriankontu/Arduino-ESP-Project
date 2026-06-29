#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <TinyGPSPlus.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define wifi_ssid "feb"
#define wifi_pswd "passwordapa"
#define API_KEY "AIzaSyAzSGDRNwlLvcSCPomMYcfihT5pNdaYXKA"
#define DATABASE_URL "https://npkmonitoring-b0d7c-default-rtdb.firebaseio.com/"

TinyGPSPlus gps;
HardwareSerial gpsSerial(2);

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;
float gps_lat;
float gps_lng;

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(9600, SERIAL_8N1, 16, 17);
  WiFi.begin(wifi_ssid, wifi_pswd);
  Serial.print("Connecting to WiFi");
  while(WiFi.status() != WL_CONNECTED){
    Serial.print("."); delay(300);
  }
  Serial.println();
  Serial.println("Connected To WiFi");

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if(Firebase.signUp(&config, &auth, "", "")){
    Serial.println("signUp OK");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  
}

void loop() {
  if(Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 5000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis - millis();
    while (gpsSerial.available()) {
    // Serial.write(gpsSerial.read());  // tampilkan semua data mentah dari GPS
    gps.encode(gpsSerial.read());
    if(gps.location.isUpdated()){
        gps_lat = gps.location.lat();
        gps_lng = gps.location.lng();
        Firebase.RTDB.setFloat(&fbdo, "GPS/latitude", gps_lat);
        Firebase.RTDB.setFloat(&fbdo, "GPS/longitude", gps_lng);
      }
    }
  }
}
