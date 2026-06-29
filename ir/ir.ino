#define ir1 45
#define ir2 47
#define ir3 49
#define ir4 51

void setup() {
  // put your setup code here, to run once:
  pinMode(ir1, INPUT);
  pinMode(ir2, INPUT);
  pinMode(ir3, INPUT);
  pinMode(ir4, INPUT);

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int state1 = !digitalRead(ir1);
  int state2 = !digitalRead(ir2);
  int state3 = !digitalRead(ir3);
  int state4 = !digitalRead(ir4);

  Serial.print(state1);
  Serial.print(", ");
  Serial.print(state2);
  Serial.print(", ");
  Serial.print(state3);
  Serial.print(", ");
  Serial.print(state4);
  Serial.println(" ");
}
