#include <Ps3Controller.h>
#include <ESP32Servo.h>

Servo servo;

// #define in1 ;
// #define in2 ;
// #define in3 ;
// #define in4 ;
// #define enA ;
// #define enB ;

void setup()
{
    Serial.begin(115200);
    Ps3.begin("38:4f:f0:8c:56:2f");
    Serial.println("Ready.");
    servo.attach(17);
    // pinMode(in1, OUTPUT);
    // pinMode(in2, OUTPUT);
    // pinMode(in3, OUTPUT);
    // pinMode(in4, OUTPUT);
    // pinMode(enA, OUTPUT);
    // pinMode(enB, OUTPUT);

}

void loop()
{
  int move;
  int rotate;
    if(Ps3.isConnected()){

        if( Ps3.data.button.cross ){
            Serial.println("Pressing the cross button");
        }

        if( Ps3.data.button.square ){
            Serial.println("Pressing the square button");
        }

        if( Ps3.data.button.triangle ){
            Serial.println("Pressing the triangle button");
        }

        if( Ps3.data.button.circle ){
            Serial.println("Pressing the circle button");
        }

        if( abs(Ps3.event.analog_changed.stick.lx) + abs(Ps3.event.analog_changed.stick.ly) > 2 ){
            Serial.print("Moved the left stick:");
            move = antidrift((Ps3.data.analog.stick.ly), 12);
            move = map(move, -128, 128, -255, 255);
            if (move > 0){
              maju(abs(move));
            }
            else if(move < 0){
              mundur(abs(move));
            }
            Serial.print(" x="); Serial.print(Ps3.data.analog.stick.lx);
            Serial.print(" y="); Serial.print(Ps3.data.analog.stick.ly);
            Serial.println();
        }

        if( abs(Ps3.event.analog_changed.stick.rx) + abs(Ps3.event.analog_changed.stick.ry) > 2 ){
            Serial.print("Moved the right stick:");
            rotate = antidrift((Ps3.data.analog.stick.rx), 12);
            rotate = map(rotate, -128, 128, -255, 255);
            if(rotate > 0){
              kanan(abs(rotate));
            }
            else if(rotate < 0){
              kiri(abs(rotate));s
            }
            Serial.print(" x="); Serial.print(Ps3.data.analog.stick.rx);
            Serial.print(" y="); Serial.print(Ps3.data.analog.stick.ry);
            Serial.println();
         }
    }
}

int maju(int speed){
  // digitalWrite(in1, HIGH);
  // digitalWrite(in2, LOW);
  // analogWrite(enA, speed);
  // digitalWrite(in3, HIGH);
  // digitalWrite(in4, LOW);
  // analogWrite(enB, speed);
}

int mundur(int speed){
  // digitalWrite(in1, LOW);
  // digitalWrite(in2, HIGH);
  // analogWrite(enA, speed);
  // digitalWrite(in3, LOW);
  // digitalWrite(in4, HIGH);
  // analogWrite(enB, speed);
}

int kanan(int speed){
  // digitalWrite(in1, HIGH);
  // digitalWrite(in2, LOW);
  // analogWrite(enA, speed);
  // digitalWrite(in3, LOW);
  // digitalWrite(in4, HIGH);
  // analogWrite(enB, speed);
}

int kiri(int speed){
  // digitalWrite(in1, LOW);
  // digitalWrite(in2, HIGH);
  // analogWrite(enA, speed);
  // digitalWrite(in3, HIGH);
  // digitalWrite(in4, LOW);
  // analogWrite(enB, speed);
}

int antidrift(int value, int max){
  	if(abs(value) < max)
	{
		return 0;
	}
	else if(value > 0)
	{
		return value - max;
	}
	else
	{
		return value + max;
	}
}
