#include <SPI.h>
#include <Wire.h>
#include <SparkFunLSM6DS3.h>

LSM6DS3 myAccGyro;

void setup() {
  myAccGyro.begin();
  Serial.begin(9600);
}

void loop() {
  //get accelerometer data
  int x_angle = calcRoll();
  //Serial.println(x_angle);
  
  if ((x_angle > 50) && (x_angle < 100)){ //i.e. accelerometer says hand is in stop position
    Serial.print('S');
  }
  else if (x_angle > 110){ //i.e. accelerometer says hand is in forward position
    Serial.print('F');
  }
  else if ((x_angle > 0) && (x_angle < 40)){ //i.e. accelerometer says hand is in reverse position
    Serial.print('R');
  }
  delay(10); //prevents overwhelming the serial port
}

int calcRoll(){ //rotation angle about x-axis
  double acc_y = myAccGyro.readFloatAccelY();
  double acc_z = myAccGyro.readFloatAccelZ();

  int roll = atan2(acc_y, acc_z)*57.3;
  return abs(roll);
}

