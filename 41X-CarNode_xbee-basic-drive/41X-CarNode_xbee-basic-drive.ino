#include <Arduino_FreeRTOS.h>
#include <AFMotor.h>

// Left Motor (A)
AF_DCMotor motor_left(1);

// Right Motor (B)
AF_DCMotor motor_right(2);

// Distance Sensor - Front
int trigPin_front = 35;
int echoPin_front = 37;

// Distance Sensor - Back
int trigPin_back = 39;
int echoPin_back = 41;

// Distance Sensor - Left
int trigPin_left = 43;
int echoPin_left = 45;

// Distance Sensor - Right
int trigPin_right = 47;
int echoPin_right = 49;


// Multi-tasking variables and methods
//bool forwardBreak = false;
//bool reverseBreak = false;
//bool leftBreak = false;
//bool rightBreak = false;
//
//bool isDriver = false;
//bool onStop = false;
//
//void TaskDrive(void *pvParameters); // communication task - 1
//void TaskBrake(void *pvParameters); // collision avoidance task - 2

/* To do:
 *  1. Re-divide tasks so that there is:
 *        a)Serial Comm task (updates a global of the last command)
 *        b)Collision avoidance task (monitors all distance sensors)
 *        c)Core task (main)
 *  2. Make left, right NOT binary (range of turning possible based on angle of sleeve accelerometer)
 *  3. Make front, reverse speed NOT binary
 *  
 * Fun things:
 *  1. Allow WALL-E to walk with you :) (i.e. accelerometer needs to recognize that the limb (i.e. arm/leg) is moving too and allows WALL-E to walk with it. Would req localization...)
 *  2. 
*/

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

//  xTaskCreate(
//    TaskDrive
//    , (const portCHAR *)"Driver"
//    , 128 // Stack Size
//    , NULL
//    , 1 // priority
//    , NULL );
//    
//  xTaskCreate(
//    TaskBrake
//    , (const portCHAR *)"Brakes"
//    , 128 // Stack Size
//    , NULL
//    , 2 // priority
//    , NULL );
}

void loop() {
  if (Serial1.available() > 0) {
    char transmittedMsg = (char)Serial1.read();
    
    if (transmittedMsg == 'S') { //Stop
      motor_left.setSpeed(0);
      motor_left.run(BRAKE);
      motor_right.setSpeed(0);
      motor_right.run(BRAKE);
      Serial.println("Stop");
    }
    else if (transmittedMsg == 'F') { //Forward
      motor_left.setSpeed(255);
      motor_left.run(FORWARD);
      motor_right.setSpeed(255);
      motor_right.run(FORWARD);
      Serial.println("Forward");
    }
    else if (transmittedMsg == 'B') { //Backwards
      motor_left.setSpeed(255);
      motor_left.run(BACKWARD);
      motor_right.setSpeed(255);
      motor_right.run(BACKWARD);
      Serial.println("Reverse");
    }
    else if (transmittedMsg == 'L') { //Left
      motor_left.setSpeed(0);
      motor_left.run(BRAKE);
      motor_right.setSpeed(255);
      motor_right.run(FORWARD);
      Serial.println("Forward");
    }
    else if (transmittedMsg == 'R') { //Right
      motor_left.setSpeed(255);
      motor_left.run(FORWARD);
      motor_right.setSpeed(0);
      motor_right.run(BRAKE);
      Serial.println("Reverse");
    }
  }
  
  // Everything done in tasks
}

//void TaskDrive(void *pvParameters) {  
//  //loop
//  while(1){
//    if (Serial.available() > 0){
//      char transmittedMsg = (char)Serial.read();
//      Serial.println(transmittedMsg);
//      if (transmittedMsg == 'S') { //Stop
//        digitalWrite(enablePin, LOW);
//        onStop = true;
//        isDriver = true;
//        //Serial.println("Stop");
//      }
//      else if ((transmittedMsg == 'F') && (!forwardBreak)) { //Forward
//        digitalWrite(enablePin, HIGH);
//        digitalWrite(IN1, LOW);
//        digitalWrite(IN2, HIGH);
//        //analogWrite(IN2, 200);
//        onStop = false;
//        isDriver=false;
//        //Serial.println("Forward");
//      }
//      else if (transmittedMsg == 'B') && (!reverseBreak)) { //Back
//        digitalWrite(enablePin, HIGH);
//        digitalWrite(IN1, HIGH);
//        digitalWrite(IN2, LOW);
//        onStop = false;
//        isDriver=true;
//        //Serial.println("Reverse");
//      }
//      else if ((transmittedMsg == 'L') && (!leftBreak)) { //Left
//        digitalWrite(enablePin, HIGH);
//        digitalWrite(IN1, LOW);
//        digitalWrite(IN2, HIGH);
//        //analogWrite(IN2, 200);
//        onStop = false;
//        isDriver=false;
//        //Serial.println("Forward");
//      }
//      else if (transmittedMsg == 'R') && (!rightBreak)) { //Right
//        digitalWrite(enablePin, HIGH);
//        digitalWrite(IN1, HIGH);
//        digitalWrite(IN2, LOW);
//        onStop = false;
//        isDriver=true;
//        //Serial.println("Reverse");
//      }
//    }
//    vTaskDelay(50/portTICK_PERIOD_MS); //100ms delay
//    
//    //Serial.flush();
//    Serial.end();
//    Serial.begin(9600);
//    vTaskDelay(20/portTICK_PERIOD_MS);
//  }
//}
//
//void TaskBrake(void *pvParameters) {
//  //setup pins for distance sensor
//  pinMode(powerPin, OUTPUT);
//  digitalWrite(powerPin, HIGH);
//  pinMode(trigPin, OUTPUT);
//  pinMode(echoPin, INPUT);
//  
//  //loop
//  while(1){
//    int distance = getDistance();
//    //if (!isDriver) {
//      if ((distance < 250) && (distance > 200)){
//        forceBreak = true;
//        //Serial.println("writing to motors - SLOW");
//        
//        //slow motor code (if it is not already stopped)
//        if ((!onStop) && (!isDriver)) {
//          digitalWrite(enablePin, HIGH);
//          digitalWrite(IN1, LOW);
//          int speedFactor = 255-5*(250-distance);
//          //int speedFactor = 0;
//          analogWrite(IN2, speedFactor);
//        }
//      }
//      else if (distance <= 200) {
//        forceBreak = true;
//        //Serial.println("writing to motors - FORCE BREAK");
//        if (!isDriver) {
//          digitalWrite(enablePin, LOW); // stop motor
//        }
//      }
//      else {
//        //Serial.println("not writing to motors - task distance");
//        forceBreak = false;
//      }
//    //}
//    //Serial.println(distance);
//    vTaskDelay(20/portTICK_PERIOD_MS); //20ms
//  }
//}

int getDistance(int trigPin, int echoPin) {
  int distance = 0;
  long time_us;
  
  // clear trigger pin
  digitalWrite(trigPin, LOW);
  vTaskDelay(0.002/portTICK_PERIOD_MS); //2us
  
  // send pulse of 10us to triger to START measuring
  digitalWrite(trigPin, HIGH);
  vTaskDelay(0.01/portTICK_PERIOD_MS); //10us
  digitalWrite(trigPin, LOW);

  // get time it takes for sound wave to echo back to sensor (in microseconds)
  time_us = pulseIn(echoPin, HIGH);

  // convert time to distance (given that speed of sound = 340m/s)
  distance = time_us/58;

  return distance;
}
