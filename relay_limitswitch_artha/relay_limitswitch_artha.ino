#define relay1 15
#define relay2 2
#define relay3 4
#define limitSwitch1 5
#define limitSwitch2 18
#define limitSwitch3 19

void setup() {
  // put your setup code here, to run once:
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(limitSwitch1, INPUT_PULLUP);
  pinMode(limitSwitch2, INPUT_PULLUP);
  pinMode(limitSwitch3, INPUT_PULLUP);

  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  int data1 = digitalRead(limitSwitch1);
  int data2 = digitalRead(limitSwitch2);
  int data3 = digitalRead(limitSwitch3);

  //Relay 1
  if(data1 == 1){
    digitalWrite(relay1, LOW);
  }
  else if(data1 == 0){
    digitalWrite(relay1, HIGH);
  }

  //Relay 2
  if(data2 == 1){
    digitalWrite(relay2, LOW);
  }
  else if(data2 == 0){
    digitalWrite(relay2, HIGH);
  }

  //Relay 3
  if(data3 == 1){
    digitalWrite(relay3, LOW);
  }
  else if(data3 == 0){
    digitalWrite(relay3, HIGH);
  }
  delay(100);

  Serial.print(data1);
  Serial.print(", ");
  Serial.print(data2);
  Serial.print(", ");
  Serial.println(data3);
}
