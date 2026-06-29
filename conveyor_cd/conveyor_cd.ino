#define enA 6
#define in1 7
#define in2 8
#define echo 9
#define trig 10

float duration, distance;

void conveyor_gerak(){
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  analogWrite(enA, 255);
}

void conveyor_berhenti(){
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  analogWrite(enA, 200);
}

void setup() {
  // put your setup code here, to run once:
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(trig, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(trig, LOW);  
	delayMicroseconds(2);  
	digitalWrite(trig, HIGH);  
	delayMicroseconds(10);  
	digitalWrite(trig, LOW);  

  duration = pulseIn(echo, HIGH);  
  distance = (duration*.0343)/2;  

  Serial.println(distance);
  delay(100);

  if(distance < 5){
    delay(3500);
    conveyor_berhenti();
    delay(1500);
    conveyor_gerak();
  }
  else{
    conveyor_gerak();
  }

}


