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
  
  delay(10); //prevents overwhelming the serial port
}

double calcRoll(){ //rotation angle about x-axis
  double acc_y = myAccGyro.readFloatAccelY();
  double acc_z = myAccGyro.readFloatAccelZ();

  double roll = atan2(acc_y, acc_z)*57.3;
  return abs(roll);
}

double calcPitch(){ //rotation angle about y-axis
  double acc_x = myAccGyro.readFloatAccelX();
  double acc_y = myAccGyro.readFloatAccelY();
  double acc_z = myAccGyro.readFloatAccelZ();

  double pitch = atan2((-acc_x), sqrt(acc_y*acc_y + acc_z*acc_z)) * 57.3;
  return pitch;
}
