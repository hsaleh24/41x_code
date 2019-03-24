#include <SPI.h>
#include <Wire.h>
#include <SparkFunLSM6DS3.h>

LSM6DS3 myAccGyro;

void setup() {
  myAccGyro.begin();
  Serial.begin(9600);
}

void loop() {
  // 1 - get accelerometer data
  double x_angle = calcRoll();
  double y_angle = calcPitch();

  // 2 - write to serial port for wireless com
  String data = (String)x_angle + "," + (String)y_angle + "*";
  Serial.println(data);
//  Serial.println("F"); // for data collection
  
  delay(300); //prevents overwhelming the serial port
}

double calcRoll(){ //rotation angle about x-axis
  double acc_y = myAccGyro.readFloatAccelY();
  double acc_z = myAccGyro.readFloatAccelZ();

//  Serial.println("ACC_Y: " + (String)acc_y);
//  Serial.println("ACC_Z: " + (String)acc_z);

  double roll = atan2(acc_y, acc_z)*57.3;
  if (acc_y < 0) // account for other quadrants
    roll += 360;

//  if (acc_z < 0) // quadrant 2 or 3
//    roll = 180 + roll;
//  else if (acc_y < 0 && acc_z > 0) // quadrant 4
//    roll = 360 + roll;
  
  return roll;
}

double calcPitch(){ //rotation angle about y-axis
  double acc_x = myAccGyro.readFloatAccelX();
  double acc_y = myAccGyro.readFloatAccelY();
  double acc_z = myAccGyro.readFloatAccelZ();

//  Serial.println("ACC_X: " + (String)acc_x);
//  Serial.println("ACC_Y: " + (String)acc_y);
//  Serial.println("ACC_Z: " + (String)acc_z);

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
