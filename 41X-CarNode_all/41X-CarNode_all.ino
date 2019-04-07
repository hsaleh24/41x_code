#include <SPI.h>
#include <Wire.h>
#include <SparkFunLSM6DS3.h>
#include <AFMotor.h>

// Left Motor (A)
AF_DCMotor motor_left(1);

// Right Motor (B)
AF_DCMotor motor_right(2);

// Accelerometer
LSM6DS3 myAccGyro;


// data collection variables
double startHome = 0.0; // desired

// drive command variables
double MAX_SPEED = 255.0;  // pwm variable
const double MAX_SPEED_ANGLE = 90.0; // accelerometer data
const double MIN_SPEED_ANGLE = 0.0;  // accelerometer data
double TURN_SPEED = 255.0; // pwm variable


// Distance Sensor - FRONT
int trigPin_front = 38;
int echoPin_front = 39;

// Distance Sensor - RIGHT
int trigPin_right = 40;
int echoPin_right = 41;

// Distance Sensor - LEFT
int trigPin_left = 36;
int echoPin_left = 37;

// Distance Sensor - back
int trigPin_back = 35;
int echoPin_back = 34;

bool forwardBrake = false;
bool reverseBrake = false;
bool leftBrake = false;
bool rightBrake = false;


void setup() {
  // setup accelerometer
  myAccGyro.begin();
  Serial.begin(9600);

  // setup XBee Com
  Serial1.begin(9600);

  // setup Distance sensors
  SetupDistanceSensors();

  // home - what is "straight"
  startHome = calcPitch();
}

void loop() {
  if (Serial1.available() > 0) {
    // 0 - collision avoidance
    MonitorDistanceSensors();
    
    // 1 - get command
    double vel = GetTransmittedMessage(",").toDouble();
    double dir = GetTransmittedMessage("*").toDouble();

    // debugging - echo serial comm data (check)
//    Serial.print(vel);
//    Serial.print(",");
    Serial.print(dir);
//    Serial.println();
    
    // 2 - Get motor speeds
    MAX_SPEED = GetMaxSpeed(vel);
//    Serial.println(MAX_SPEED); // debugging
    TURN_SPEED = GetMotorTurnSpeed(dir);
    Serial.println(TURN_SPEED);

    // 3 - drive motors
    int quadrant = GetQuadrant(dir);
    switch (quadrant)
    {
      case 0:
        // collision avoidance
        if (dir <= 20 && forwardBrake)
        {
          Brake();
          break;
        }
        else if (dir > 20 && dir < 70 && (forwardBrake || rightBrake))
        {
          Brake();
          break;
        }
        else if (dir >= 70 && rightBrake)
        {
          Brake();
          break;
        }
        motor_left.setSpeed(MAX_SPEED);
        motor_left.run(FORWARD);
        TurnMotor(motor_right);
        break;
      case 1:
        // collision avoidance
        if (dir <= 110 && rightBrake)
        {
          Brake();
          break;
        }
        else if (dir > 110 && dir < 160 && (rightBrake || reverseBrake))
        {
          Brake();
          break;
        }
        else if (dir >= 160 && reverseBrake)
        {
          Brake();
          break;
        }
        motor_left.setSpeed(MAX_SPEED);
        motor_left.run(BACKWARD);
        TurnMotor(motor_right);
        break;
      case 2:
        // collision avoidance
        if (dir <= 200 && reverseBrake)
        {
          Brake();
          break;
        }
        else if (dir > 200 && dir < 250 && (reverseBrake || leftBrake))
        {
          Brake();
          break;
        }
        else if (dir >= 250 && leftBrake)
        {
          Brake();
          break;
        }
        motor_right.setSpeed(MAX_SPEED);
        motor_right.run(BACKWARD);
        TurnMotor(motor_left);
        break;
      case 3:
        // collision avoidance
        if (dir <= 290 && leftBrake)
        {
          Brake();
          break;
        }
        else if (dir > 290 && dir < 340 && (leftBrake || frontBrake))
        {
          Brake();
          break;
        }
        else if (dir >= 340 && frontBrake)
        {
          Brake();
          break;
        }
        motor_right.setSpeed(MAX_SPEED);
        motor_right.run(FORWARD);
        TurnMotor(motor_left);
        break;
      default:
        break;
    }
    
    // 4 - accelerometer data (data logging)
//    double actualAngle = calcPitch();
//    Serial.print(startHome);
//    Serial.print(" ");
//    Serial.println(actualAngle);
  }
  delay(50);
}

void Brake()
{
  motor_left.setSpeed(0);
  motor_left.run(BRAKE);
  motor_right.setSpeed(0);
  motor_right.run(BRAKE);
}

void TurnMotor(AF_DCMotor turn_motor)
{
  turn_motor.setSpeed(abs(TURN_SPEED));
  if (TURN_SPEED >= 0)
    turn_motor.run(FORWARD);
  else if (TURN_SPEED < 0)
    turn_motor.run(BACKWARD);
}

double GetMaxSpeed(double vel)
{
  if (vel > 45)
    return 0.0;
  else
    return ((90.0-vel)/(MAX_SPEED_ANGLE - MIN_SPEED_ANGLE))*255.0; // maps angle range to appropriate PWM speed
}

double GetMotorTurnSpeed(double dir)
{
  // get the quadrant
  int quadrant = GetQuadrant(dir);

  // get sign
  int sign = 1;
  if (quadrant == 2 || quadrant == 3)
    sign = -1; 

  // get secondary motor speed factor (pivot wheel in the turn)
  double deg = dir-(quadrant*90.0);
  int speedFactor = map(deg, 0, 90, sign*MAX_SPEED, -1*sign*MAX_SPEED);

  return speedFactor;
}

int GetQuadrant(double dir)
{
  int quadrant = 0;
  if (dir >= 90)
    quadrant = ((int)dir - ((int)dir % 90))/90; // returns 1, 2, or 3

//  Serial.print("quadrant: "); // debugging
//  Serial.println(quadrant); // debugging
  
  return quadrant;
}

double calcRoll(){ //rotation angle about x-axis
  double acc_y = myAccGyro.readFloatAccelY();
  double acc_z = myAccGyro.readFloatAccelZ();

  double roll = atan2(acc_y, acc_z)*57.3;
  if (acc_y < 0) // account for other quadrants
    roll += 360;
  
  return roll;
}

double calcPitch(){ //rotation angle about y-axis
  double acc_x = myAccGyro.readFloatAccelX();
  double acc_y = myAccGyro.readFloatAccelY();
  double acc_z = myAccGyro.readFloatAccelZ();

  double pitch = atan2((-acc_x), sqrt(acc_y*acc_y + acc_z*acc_z)) * 57.3;

  // account for other quadrants
  if (acc_x < 0 && acc_z > 0)
    pitch = 180 - pitch;
  else if (acc_x > 0 && acc_z > 0)
    pitch = 180 - pitch;
  else if (acc_x > 0 && acc_z < 0)
    pitch += 360;
  
  return pitch;
}

String GetTransmittedMessage(String endChar) // POST: returns string WITHOUT the end char
{
  String transmittedMsg = "";
  
  while (!transmittedMsg.endsWith(endChar))
  {
    if (Serial1.available() > 0)
    {
      transmittedMsg += (char)Serial1.read();
    }
  }
  transmittedMsg[transmittedMsg.length() - 1] = '\0'; // remove the end char (replace with null)
  transmittedMsg.trim(); // removing any possible trailing white space (e.g. '\n')
  
  return transmittedMsg;
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

void MonitorDistanceSensors() {
  if (GetDistance(trigPin_front,echoPin_front) < 20)
    forwardBrake = true;
  else
    forwardBrake = false;
    
  if (GetDistance(trigPin_back,echoPin_back) < 20)
    reverseBrake = true;
  else
    reverseBrake = false;
  
  if (GetDistance(trigPin_right,echoPin_right) < 20)
    rightBrake = true;
  else
    rightBrake = false;
  
  if (GetDistance(trigPin_left,echoPin_left) < 20)
    leftBrake = true;
  else
    leftBrake = false;
}

int GetDistance(int trigPin, int echoPin) {
//  Serial.println("DEBUG - GetDistance");
  
  int distance = 0;
  long time_us;
  
  // clear trigger pin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2); //2us
  
  // send pulse of 10us to triger to START measuring
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); //10us
  digitalWrite(trigPin, LOW);

  // get time it takes for sound wave to echo back to sensor (in microseconds)
  time_us = pulseIn(echoPin, HIGH);

  // convert time to distance (given that speed of sound = 340m/s)
  distance = time_us/58;

  return distance;
}
