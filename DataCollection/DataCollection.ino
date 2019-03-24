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
//    char transmittedMsg = (char)Serial1.read();
    double vel = GetTransmittedMessage(",").toDouble();
    double dir = GetTransmittedMessage("*").toDouble();
//    double offset = 0.0;

    // 2 - drive motors
//    if (transmittedMsg == 'S') { //Stop
//      motor_left.setSpeed(0);
//      motor_left.run(BRAKE);
//      motor_right.setSpeed(0);
//      motor_right.run(BRAKE);
//    }
    if ((dir > 315 && dir <= 360)||(dir >= 0 && dir <= 45)) { //Forward
      motor_left.setSpeed(255);
      motor_left.run(FORWARD);
      motor_right.setSpeed(255);
      motor_right.run(FORWARD);
    }
    else if (dir > 135 && dir <= 225) { //Backwards
      motor_left.setSpeed(255);
      motor_left.run(BACKWARD);
      motor_right.setSpeed(255);
      motor_right.run(BACKWARD);
//      offset = 180;
    }
    else if (dir > 225 && dir <= 315){ //Left
      motor_left.setSpeed(0);
      motor_left.run(BRAKE);
      motor_right.setSpeed(255);
      motor_right.run(FORWARD);
//      offset = 270;
    }
    else if (dir > 45 && dir <= 135) { //Right
      motor_left.setSpeed(255);
      motor_left.run(FORWARD);
      motor_right.setSpeed(0);
      motor_right.run(BRAKE);
//      offset = 360;
    }

    // 3 - accelerometer data
    double actualAngle = calcPitch();
    Serial.print(startHome);
    Serial.print(" ");
    Serial.println(actualAngle);
  }
  delay(100);
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
