#define BLYNK_TEMPLATE_ID "TMPL69wYMWYKs"
#define BLYNK_TEMPLATE_NAME "TUBES IOT"
#define BLYNK_AUTH_TOKEN "8t1ELV0RQueh4uBSE8AmuIuWg9pB6faC"

#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h>

#define sensorAir 35
#define sensorGas 34
#define sensorLDR 32
#define buzzer 33
#define button_Benar 12
#define button_Salah 13
#define LED 14

WidgetTerminal terminal(V3);

Servo servo_jendela;
Servo servo_pintu;
int air= 0 ;
int gas = 0;
int cahaya = 0;
int button_benar = 0;
int button_salah = 0;

char auth[] = "8t1ELV0RQueh4uBSE8AmuIuWg9pB6faC";
char ssid[] = "feb";
char pass[] = "passwordapa";
String data;

BLYNK_CONNECTED() 
{
  Blynk.sendInternal("rtc", "sync")
  Blynk.syncVirtual(V0);
  Blynk.syncVirtual(V1);
  Blynk.syncVirtual(V2);
  Blynk.syncVirtual(V3);
}

BLYNK_WRITE(V0)
{
digitalWrite(LED, param.asInt());
}

BLYNK_WRITE(V1)
{
 servo_jendela.write(param.asInt());
 if(param.asInt() == 0){
  Blynk.virtualWrite(V3, "Membuka Jendela");
 }
else if(param.asInt() == 130){
  Blynk.virtualWrite(V3, "Menutup Jendela");
}
//  Blynk.virtualWrite()
}

BLYNK_WRITE(V2)
{
servo_pintu.write(param.asInt());
 if(param.asInt() == 10){
  Blynk.virtualWrite(V3, "Membuka Pintu");
 }
 else if(param.asInt() == 150){
  Blynk.virtualWrite(V3, "Menutup Pintu");
}
}

BLYNK_WRITE(V3)
{
data = param.asStr();
}



void setup()
{
  pinMode(sensorGas, INPUT);
  pinMode(sensorAir, INPUT);
  pinMode(sensorLDR, INPUT);
  pinMode(button_Benar, INPUT);
  pinMode(button_Salah, INPUT);
  pinMode(LED, OUTPUT);
  pinMode(buzzer, OUTPUT);
  servo_jendela.attach(27);
  servo_pintu.attach(26);
  Serial.begin(115200);

  WiFi.begin(ssid, pass);
  int wifi_ctr = 0;
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.println(".");
  }
  Serial.println("WiFi connected");
  Blynk.begin(auth, ssid, pass);
}

void loop()
{
  Blynk.run();
  air = analogRead(sensorAir);
  button_benar = digitalRead(button_Benar);
  button_salah = digitalRead(button_Salah);
  gas = analogRead(sensorGas);
  cahaya = analogRead(sensorLDR);
  Serial.println(cahaya);
  sensor_air();
 pintu();
 sensor_gas();
 LDR();
}
  
  void sensor_air(){
  air = analogRead(sensorAir);
  if(air >= 1000){
    servo_jendela.write(130);
    }
  else if(air < 1000 && air > 350){
    servo_jendela.write(0);
}
  }
  
  void pintu(){
  button_benar = digitalRead(button_Benar);
  button_salah = digitalRead(button_Salah);
    
    if(button_benar == 1 && button_salah == 0){
      servo_pintu.write(0);
      noTone(buzzer);
    }
    else if(button_benar == 0 && button_salah == 1){
      servo_pintu.write(150);
      tone(buzzer, 1000);
      delay(2000);
    }
  }
  
  void sensor_gas(){
     gas = analogRead(sensorGas);
     if(gas >= 2000){
     tone(buzzer, 1000);
     delay(2000);
     }
     else{
     noTone(buzzer);
     }
  }
	
    void LDR(){
	  cahaya = analogRead(sensorLDR);
      if(cahaya >= 1000 && cahaya < 3500){
		digitalWrite(LED, HIGH);
      }
      else if(cahaya < 1000 && cahaya > 550){
		digitalWrite(LED, LOW);
      }
    }
    