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
//  int z_angle = calcYaw();
  int y_angle = calcPitch();
  Serial.print(x_angle + "," + y_angle + "*");
  
//  if ((x_angle > 50) && (x_angle < 100)){ //i.e. accelerometer says hand is in stop position
//    Serial.print('S');
//  }
//  else if (x_angle > 110){ //i.e. accelerometer says hand is in forward position
//    Serial.print('F');
//  }
//  else if ((x_angle > 0) && (x_angle < 40)){ //i.e. accelerometer says hand is in reverse position
//    Serial.print('R');
//  }
  delay(10); //prevents overwhelming the serial port
}

int calcRoll(){ //rotation angle about x-axis
  double acc_y = myAccGyro.readFloatAccelY();
  double acc_z = myAccGyro.readFloatAccelZ();

  int roll = atan2(acc_y, acc_z)*57.3;

  if (roll < 0)
    return 0;
  else
    return abs(roll);
}

int calcPitch(){ //rotation angle about y-axis
  double acc_x = myAccGyro.readFloatAccelX();
  double acc_y = myAccGyro.readFloatAccelY();
  double acc_z = myAccGyro.readFloatAccelZ();

  int pitch = atan2((-acc_x), sqrt(acc_y*acc_y + acc_z*acc_z)) * 57.3;
  
  if (pitch < 0)
    return 0;
  else
    return abs(pitch);
}

// FUNCTION BELOW IS NOT CORRECT - DID NOT UPDATE THE EQN
int calcYaw(){ //rotation angle about z-axis
  double acc_x = myAccGyro.readFloatAccelX();
  double acc_y = myAccGyro.readFloatAccelX();

  int yaw = atan2(acc_x, acc_y)*57.3;

  if (yaw < 0)
    return 0;
  else
    return abs(yaw);
}
