/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-rotary-encoder
 */

#include <ezButton.h>  // the library to use for SW pin

#define CLK_PIN 25 // ESP32 pin GPIO25 connected to the rotary encoder's CLK pin
#define DT_PIN  26 // ESP32 pin GPIO26 connected to the rotary encoder's DT pin
#define SW_PIN  27 // ESP32 pin GPIO27 connected to the rotary encoder's SW pin
#define limitSwitchAtas 5
#define limitSwitchBawah 18 

#define DIRECTION_CW  0   // clockwise direction
#define DIRECTION_CCW 1  // counter-clockwise direction

int counter = 0;
int direction = DIRECTION_CW;
int CLK_state;
int prev_CLK_state;
unsigned long lastState = 0;

const int COUNTER_MIN = 0;
const int COUNTER_MAX = 125;

ezButton button(SW_PIN);  // create ezButton object that attach to pin 7;

void setup() {
  Serial.begin(115200);

  // configure encoder pins as inputs
  pinMode(CLK_PIN, INPUT_PULLUP);
  pinMode(DT_PIN, INPUT_PULLUP);
  pinMode(limitSwitchAtas, INPUT_PULLUP);
  pinMode(limitSwitchBawah, INPUT_PULLUP);
  button.setDebounceTime(50);  // set debounce time to 50 milliseconds

  // read the initial state of the rotary encoder's CLK pin
  prev_CLK_state = digitalRead(CLK_PIN);
}

void loop() {
  button.loop();  // MUST call the loop() function first
  int dataLimitAtas = digitalRead(limitSwitchAtas);
  int dataLimitBawah = digitalRead(limitSwitchBawah);

  if(dataLimitAtas == 1){
    counter = COUNTER_MIN;
  }
  
  if(dataLimitBawah == 1){
    counter = COUNTER_MAX;
  }
  // read the current state of the rotary encoder's CLK pin
  CLK_state = digitalRead(CLK_PIN);
  
  int persen = map(counter, COUNTER_MIN, COUNTER_MAX, 0, 100);

  unsigned long now = millis();
  // If the state of CLK is changed, then pulse occurred
  // React to only the rising edge (from LOW to HIGH) to avoid double count
  if (CLK_state != prev_CLK_state && CLK_state == HIGH) {
    // if the DT state is HIGH
    // the encoder is rotating in counter-clockwise direction => decrease the counter
    if (digitalRead(DT_PIN) == HIGH) {
      counter--;
      direction = DIRECTION_CCW;
    } else {
      // the encoder is rotating in clockwise direction => increase the counter
      counter++;
      direction = DIRECTION_CW;
    }


    // Serial.print("Rotary Encoder:: direction: ");
    // if (direction == DIRECTION_CW)
    //   Serial.print("Clockwise");
    // else
    //   Serial.print("Counter-clockwise");

    // Serial.print(" - count: ");
}
if(now - lastState >= 100){
    lastState = now;
    Serial.print(counter);
    Serial.print(", ");
    Serial.println(persen);
  }
  prev_CLK_state = CLK_state;



  // save last CLK state

  if (button.isPressed()) {
    Serial.println("The button is pressed");
  }
}
