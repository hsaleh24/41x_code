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
double vel = 0.0;
double dir = 0.0;

bool forwardBrake = false;
bool reverseBrake = false;
bool leftBrake = false;
bool rightBrake = false;
//
//bool isDriver = false;
//bool onStop = false;

void TaskSerialCom(void *pvParameters); // communication task - 1
void TaskCollisionDetection(void *pvParameters); // collision avoidance task - 2
void TaskMain(void *pvParameters); // main task - 3

/* To do:
 *  1. Re-divide tasks so that there is: - DONE (NOT TESTED)
 *        a)Serial Comm task (updates a global of the last command)
 *        b)Collision avoidance task (monitors all distance sensors)
 *        c)Core task (main)
 *  2. Make left, right NOT binary (range of turning possible based on angle of sleeve accelerometer)
 *  3. Make front, reverse speed NOT binary - DONE (NOT TESTED)
 *  
 * Fun things:
 *  1. Allow WALL-E to walk with you :) (i.e. accelerometer needs to recognize that the limb (i.e. arm/leg) is moving too and allows WALL-E to walk with it. Would req localization...)
 *  2. 
*/

void setup() {
  Serial.begin(9600);

  xTaskCreate(
    TaskSerialCom
    , (const portCHAR *)"XBeeSerialCom"
    , 128 // Stack Size
    , NULL
    , 3 // priority
    , NULL );
    
  xTaskCreate(
    TaskCollisionDetection
    , (const portCHAR *)"CollisionAvoidance"
    , 128 // Stack Size
    , NULL
    , 2 // priority
    , NULL );

  xTaskCreate(
    TaskMain
    , (const portCHAR *)"Main"
    , 128 // Stack Size
    , NULL
    , 1 // priority
    , NULL );
}

void loop() {
  // Everything done in tasks
}

void TaskMain(void *pvParameters) {
  // drive robot
  while(1) {
    // 1 - calc PWM signal based on accelerometer data
    int speedFactor = 255*vel/90;
    if (speedFactor > 255) // saturation
      speedFactor = 255;
    else if (speedFactor < 0)
      speedFactor = 0;

    /*update below
    * 1. Calibration values
    * 2. Make direction a range
    */
//    Serial.println(dir);
//    Serial.println(vel);
//    Serial.println(speedFactor);
    
    // 2 - write motors
    if ( ((dir > 315 && dir <= 360)||(dir >= 0 && dir <= 45)) && (!forwardBrake) ){ //Forward
      motor_left.setSpeed(speedFactor);
      motor_left.run(FORWARD);
      motor_right.setSpeed(speedFactor);
      motor_right.run(FORWARD);
      Serial.println("Debug - FORWARD");
    }
    else if ( (dir > 45 && dir <= 135) && (!rightBrake) ){ //Right
      motor_left.setSpeed(speedFactor);
      motor_left.run(FORWARD);
      motor_right.setSpeed(0);
      motor_right.run(BRAKE);
      Serial.println("Debug - RIGHT");
    }
    else if ( (dir > 135 && dir <= 225) && (!reverseBrake) ){ //Backwards
      motor_left.setSpeed(speedFactor);
      motor_left.run(BACKWARD);
      motor_right.setSpeed(speedFactor);
      motor_right.run(BACKWARD);
      Serial.println("Debug - REVERSE");
    }
    else if ( (dir > 225 && dir <= 315) && (!leftBrake) ){ //Left
      motor_left.setSpeed(0);
      motor_left.run(BRAKE);
      motor_right.setSpeed(speedFactor);
      motor_right.run(FORWARD);
      Serial.println("Debug - LEFT");
    }

    // 3 - collision brake
    if (forwardBrake || reverseBrake || leftBrake || rightBrake) {
      motor_left.setSpeed(0);
      motor_left.run(BRAKE);
      motor_right.setSpeed(0);
      motor_right.run(BRAKE);
    }
        
    vTaskDelay(20/portTICK_PERIOD_MS); //20ms
  }
}

void TaskSerialCom(void *pvParameters) {
  // setup serial port
  Serial1.begin(9600);
  
  // monitor serial port (and update global flags)
  while(1) {
    if (Serial1.available() > 0)
    {
      vel = GetTransmittedMessage(",").toDouble();
      dir = GetTransmittedMessage("*").toDouble();

      Serial.println(dir);
      Serial.println(vel);
    }
    Serial.println("DEBUG - XBee Task");
    vTaskDelay(20/portTICK_PERIOD_MS); //20ms
  }
}

void TaskCollisionDetection(void *pvParameters) {  
  // setup distance sensors
//  SetupDistanceSensors();

  // monitor distance sensors
  while(1) {
//    int trigPin = trigPin_front;
//    int echoPin = echoPin_front;
//    
//    for (int i=0; i<4; i++) {
//      // 1 - get sensor reading
//      int distance = GetDistance(trigPin, echoPin);
//
//      // 2 - decide to brake based on sensor reading
//      bool brake = false;
//      if (distance <= 200) // FORCE BRAKE
//        brake = true;
//
//      // 3 - update flag
//      switch (trigPin){
//        case 35: // front
//          forwardBrake = brake;
//          break;
//        case 39: // back
//          reverseBrake = brake;
//          break;
//        case 43: // left
//          leftBrake = brake;
//          break;
//        case 47: // right
//          rightBrake = brake;
//          break;
//      }
//
//      // 4 - update pin values
//      trigPin += 4;
//      echoPin += 4;
//    }
    Serial.println("DEBUG - Distance Task");
    vTaskDelay(20/portTICK_PERIOD_MS); //20ms
  }
}

void SetupDistanceSensors() {
  // Front Distance Sensor
  pinMode(trigPin_front, OUTPUT);
  pinMode(echoPin_front, INPUT);

  // Back Distance Sensor
  pinMode(trigPin_back, OUTPUT);
  pinMode(echoPin_back, INPUT);

  // Left Distance Sensor
  pinMode(trigPin_left, OUTPUT);
  pinMode(echoPin_left, INPUT);

  // Right Distance Sensor
  pinMode(trigPin_right, OUTPUT);
  pinMode(echoPin_right, INPUT);
}

int GetDistance(int trigPin, int echoPin) {
  Serial.println("DEBUG - GetDistance");
  
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

String GetTransmittedMessage(String endChar) // POST: returns string WITHOUT the end char
{
  Serial.println("DEBUG - GetTransmittedMessage");

  String transmittedMsg = "";
  
  while (!transmittedMsg.endsWith(endChar))
  {
//    Serial.println("debug - Stuck here question mark ???");
    
    if (Serial1.available() > 0)
    {
      transmittedMsg += (char)Serial1.read();
      Serial.println("debug - ever get here ?");
      Serial.println(transmittedMsg);
    }
  }
  transmittedMsg[transmittedMsg.length() - 1] = '\0'; // remove the end char (replace with null)
  transmittedMsg.trim(); // removing any possible trailing white space (e.g. '\n')
  
  return transmittedMsg;
}
