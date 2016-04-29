/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Sweep
*/

#include <Servo.h>

#define PIN_AIO1 A0
#define PIN_AIO2 A1
#define PIN_AIO3 A2
#define PIN_AIO4 A3
#define PIN_D9 9
#define PIN_D10 10
#define PIN_D13 13
#define PIN_D5_LEFT_WHEEL 5
#define PIN_D6_RIGHT_WHEEL 6

#define IR_DELAY 20 //50Hz
#define BLINKY_DELAY 400 //100ms
#define DELAY 25 //40ms
#define PROXIMITY_INTERVAL 150 //150ms
#define DRIVE_INTERVAL 10

#define DIRECTION_LEFT 0
#define DIRECTION_RIGHT 1
#define DIRECTION_FWD 0
#define DIRECTION_REV 1
#define DIRECTION_STOP 2

#define IR_THRESHOLD 590

Servo myservo_d9;  // create servo object to control a servo
Servo myservo_d10;  // create servo object to control a servo
Servo wheel_left;  // create servo object to control a servo
Servo wheel_right;  // create servo object to control a servo
// twelve servo objects can be created on most boards

volatile int pos_wheel_left = 0;
volatile int pos_wheel_right = 0;
volatile int pos_d9 = 90;    // variable to store the servo position
volatile int pos_d10 = 0;    // variable to store the servo position

//IR sensor values
volatile int ir1 = 0;
volatile int ir2 = 0;
volatile int ir3 = 0;
volatile int ir4 = 0;

volatile unsigned long millisHead = 0;
volatile unsigned long millisLED = 0;
volatile unsigned long millisIR = 0;
volatile unsigned long millisProx = 0; //proximity
volatile unsigned long millisLast = 0; //proximity

volatile int headDirection = DIRECTION_RIGHT;
volatile int leftDirection = DIRECTION_STOP;
volatile int rightDirection = DIRECTION_STOP;
volatile int ledState = HIGH;

void setup() {
  
  Serial.begin(19200);
  
  unsigned long currentMillis = millis();
  myservo_d9.attach(PIN_D9);  // attaches the servo on pin 9 to the servo object
  myservo_d9.write(pos_d9); //set to 0-deg
  millisHead = currentMillis;
  myservo_d10.attach(PIN_D10);  // attaches the servo on pin 9 to the servo object

  pinMode(PIN_D13, OUTPUT);
  digitalWrite(PIN_D13, ledState);
  millisLED = currentMillis;

  millisIR = currentMillis;
  millisProx = currentMillis;
}

void loop() {

  unsigned long currentMillis = millis();
  //
  // SERVO HEAD
//  if (currentMillis - millisHead >= DELAY) {
//    // turn head
//    millisHead = currentMillis;
//    if (headDirection == DIRECTION_RIGHT) {
//      if (pos_d9 < 180) {
//        myservo_d9.write(++pos_d9); //increment position
//      } else if (pos_d9 == 180) {
//        headDirection = DIRECTION_LEFT; //change direction
//        myservo_d9.write(--pos_d9); //decrement position
//      }
//    } else if (headDirection == DIRECTION_LEFT) {
//      if (pos_d9 > 0) {
//        myservo_d9.write(--pos_d9); //increment position
//      } else if (pos_d9 == 0) {
//        headDirection = DIRECTION_RIGHT; //change direction
//        myservo_d9.write(++pos_d9); //decrement position
//      }
//    }
//  }
  //
  // BLINKY
  if (currentMillis - millisLED >= BLINKY_DELAY) {
    millisLED = currentMillis;
    if (ledState == HIGH)
      ledState = LOW;
    else if (ledState == LOW)
      ledState = HIGH;
    digitalWrite(PIN_D13, ledState);
  }
  //
  // Sample IRs
  if (currentMillis - millisIR >= IR_DELAY) {
    millisIR = currentMillis;
    ir1 = analogRead(PIN_AIO1);
    ir2 = analogRead(PIN_AIO2);
    ir3 = analogRead(PIN_AIO3);
    ir4 = analogRead(PIN_AIO4);
    // print the results to the serial monitor:
    Serial.print("sensor-ir1=");
    Serial.print(ir1);
    Serial.print(",sensor-ir2=");
    Serial.print(ir2);
    Serial.print(",sensor-ir3=");
    Serial.print(ir3);
    Serial.print(",sensor-ir4=");
    Serial.println(ir4);
  }
  //
  // Proximity
  if (currentMillis - millisProx >= PROXIMITY_INTERVAL) {
    millisProx = currentMillis;
    if (ir1 >= IR_THRESHOLD && 
        ir3 < IR_THRESHOLD && 
        rightDirection == DIRECTION_STOP) {
      //move right wheel reverse
      rightDirection = DIRECTION_REV;
      myservo_d9.write(125);
    } else if (ir1 < IR_THRESHOLD &&
        ir3 >= IR_THRESHOLD && 
        rightDirection == DIRECTION_STOP) {
      //move right wheel forward
      rightDirection = DIRECTION_FWD;
      myservo_d9.write(1);
    } else if (ir1 < IR_THRESHOLD && ir3 < IR_THRESHOLD) {
      rightDirection = DIRECTION_STOP;
    }
    
    if (ir2 >= IR_THRESHOLD && 
      ir4 < IR_THRESHOLD && 
      leftDirection == DIRECTION_STOP) {
      //move left wheel forward
      leftDirection = DIRECTION_REV;
      myservo_d9.write(45);
    } else if (ir2 < IR_THRESHOLD &&
      ir4 >= IR_THRESHOLD && leftDirection == DIRECTION_STOP) {
      //move left wheel forward
      leftDirection = DIRECTION_FWD;
      myservo_d9.write(179);
    } else if (ir2 < IR_THRESHOLD && ir4 < IR_THRESHOLD){
      leftDirection = DIRECTION_STOP;
    }
  }
  //
  // Drive
  if (currentMillis - millisLast >= DRIVE_INTERVAL) {
    millisLast = currentMillis;
    // turn wheels
    if (leftDirection == DIRECTION_FWD) {
      wheel_left.attach(PIN_D5_LEFT_WHEEL);
      wheel_left.write(1);
    } else if (leftDirection == DIRECTION_REV) {
      wheel_left.attach(PIN_D5_LEFT_WHEEL);
      wheel_left.write(180);
    } else {
      wheel_left.detach();
    }

    // turn wheels
    if (rightDirection == DIRECTION_FWD) {
      wheel_right.attach(PIN_D6_RIGHT_WHEEL);
      wheel_right.write(180); //the right servo is reverse of the left
    } else if (rightDirection == DIRECTION_REV) {
      wheel_right.attach(PIN_D6_RIGHT_WHEEL);
      wheel_right.write(1); 
    } else {
      wheel_right.detach();
    }
  }
  
}

