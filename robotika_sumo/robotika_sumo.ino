#define in3 3
#define in4 4
#define enB 5
#define enA 6
#define in1 7
#define in2 8
#define echo 9
#define trig 10

float duration, distance;

void maju_full(){
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  analogWrite(enA, 255);

  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(enB, 255);
}

void maju_biasa(){
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  analogWrite(enA, 175);

  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(enB, 175);
}

void setup() {
  // put your setup code here, to run once:
  pinMode(enB, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(trig, OUTPUT);
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

  if(distance < 50){
    maju_full();
  }
  else{
    maju_biasa();
  }
}


