#include <SPI.h>
#include <Wire.h>
#include <SparkFunLSM6DS3.h>

LSM6DS3 myAccGyro;

void setup() {
  myAccGyro.begin();
  Serial.begin(9600);
}

/* everything only works for values of up to x=45 */
void loop() {
  // 1 - get accelerometer data
  double theta_x = 0.0;
  double theta_y = 0.0;
  for (int i =0; i<20; i++) {
    theta_x += calcRoll();
    theta_y += calcPitch();
  }
  theta_x = theta_x/20.0;
  theta_y = theta_y/20.0;

  // 2 - write to serial port for wireless com
  String data = (String)theta_x + "," + (String)theta_y + "*";
  Serial.println(data);
  
  delay(10); //prevents overwhelming the serial port
}

double calcRoll(){ //rotation angle about x-axis
  double acc_x = myAccGyro.readFloatAccelX();
  double acc_y = myAccGyro.readFloatAccelY();
  double acc_z = myAccGyro.readFloatAccelZ();

  // debugging
//  Serial.println(acc_y); // for roll (rot x)
//  Serial.println(acc_x); // for pitch (rot y)
//Serial.println(acc_z);

  double roll = acc_y*100.0;

  double pitchTemp = calcPitch();
  if (pitchTemp > 91 && pitchTemp < 269) // reverse
    acc_z = -1*acc_z;
  
  // below conditioning just for WALL-E robot :)
  if (roll < 0)
    roll = 0.0;
  else if (roll > 90 || acc_z > 0)
    roll = 90.0;
  
  return roll;
}

double calcPitch(){ //rotation angle about y-axis
  double acc_x = myAccGyro.readFloatAccelX();
  double acc_z = myAccGyro.readFloatAccelZ();

  double pitch = abs(acc_x*90.0);
  if (acc_z < 0 && acc_x < 0) // quadrant 1
    pitch = abs(pitch);
  else if (acc_z >= 0 && acc_x < 0) // quadrant 2
    pitch = 180 - pitch;
  else if (acc_z >= 0 && acc_x >= 0) // quadrant 3
    pitch = 180 + pitch;
  else if (acc_z < 0 && acc_x >= 0) // quadrant 4
    pitch = 360 - pitch;

  return pitch;
}
