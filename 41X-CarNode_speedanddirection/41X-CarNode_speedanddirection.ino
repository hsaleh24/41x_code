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

void setup() {
  // setup accelerometer
  myAccGyro.begin();
  Serial.begin(9600);

  // setup XBee Com
  Serial1.begin(9600);

  // home - what is "straight"
  startHome = calcPitch();
}

void loop() {
  if (Serial1.available() > 0) {
    // 1 - get command
    double vel = GetTransmittedMessage(",").toDouble();
    double dir = GetTransmittedMessage("*").toDouble();
    if (dir < 90)
      dir = 90.0;
    else if (dir > 180)
      dir = 180.0;
//    double offset = 0.0;

    // 2 - Get motor speeds
    MAX_SPEED = GetMaxSpeed(vel);
    TURN_SPEED = GetMotorTurnSpeed(dir);

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
    double actualAngle = calcPitch();
    Serial.print(startHome);
    Serial.print(" ");
    Serial.println(actualAngle);
  }
  delay(100);
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
  return (vel/(MAX_SPEED_ANGLE - MIN_SPEED_ANGLE))*255.0; // maps angle range to appropriate PWM speed
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
