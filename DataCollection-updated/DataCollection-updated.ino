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


void setup() {
  // setup accelerometer
  myAccGyro.begin();
  Serial.begin(9600);

  // home - what is "straight"
  startHome = calcPitch();
}

void loop() {
  // 1 - get command
  double vel = 0.01; // max speed
  double dir = 0.01;

  // 2 - Get motor speeds
  MAX_SPEED = GetMaxSpeed(vel);
  TURN_SPEED = GetMotorTurnSpeed(dir);
//  Serial.println(TURN_SPEED);

  // 3 - drive motors
  int quadrant = GetQuadrant(dir);
  switch (quadrant)
  {
    case 0:
      motor_left.setSpeed(MAX_SPEED);
      motor_left.run(FORWARD);
      TurnMotor(motor_right);
      break;
    case 1:
      motor_left.setSpeed(MAX_SPEED);
      motor_left.run(BACKWARD);
      TurnMotor(motor_right);
      break;
    case 2:
      motor_right.setSpeed(MAX_SPEED);
      motor_right.run(BACKWARD);
      TurnMotor(motor_left);
      break;
    case 3:
      motor_right.setSpeed(MAX_SPEED);
      motor_right.run(FORWARD);
      TurnMotor(motor_left);
      break;
    default:
      break;
  }
  
  // 4 - accelerometer data (data logging)
  Serial.print(dir);
  Serial.print(" ");
  double actualAngle = calcPitch();
//  if (startHome <= 180)
//    actualAngle = actualAngle
//  Serial.print(startHome);
//  Serial.print(" ");
  Serial.println(actualAngle);

  delay(500);
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
