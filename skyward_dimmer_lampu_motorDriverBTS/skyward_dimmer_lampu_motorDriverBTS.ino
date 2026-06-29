#define pwm 11
#define Rpwm 2
#define Lpwm 3

void setup() {
  // put your setup code here, to run once:
  // pinMode(pwm, OUTPUT);
  pinMode(Rpwm, OUTPUT);
  pinMode(Lpwm, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(Rpwm, HIGH);
  digitalWrite(Lpwm, LOW);

  for(int i = 0 ; i <= 255 ; i++){
    analogWrite(pwm, i);
    delay(20);
  }

  delay(1000);
    
  for(int i = 255 ; i >= 0 ; i--){
    analogWrite(pwm, i);
    delay(20);
  }

}
