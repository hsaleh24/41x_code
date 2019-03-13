#include <Arduino_FreeRTOS.h>

int enablePin = 13;
int IN1 = 12;
int IN2 = 11;

int powerPin = 9;
int trigPin = 8;
int echoPin = 7;
bool forceBreak = false;
bool isDriver = false;
bool onStop = false;

void TaskDrive(void *pvParameters);
void TaskBrake(void *pvParameters);

void setup() {
  //setup pins for motor control
  pinMode(enablePin, OUTPUT);
  digitalWrite(enablePin, LOW);
  pinMode(IN1, OUTPUT);
  digitalWrite(IN1, LOW);
  pinMode(IN2, OUTPUT);
  digitalWrite(IN2, LOW);

  Serial.begin(9600);

  xTaskCreate(
    TaskDrive
    , (const portCHAR *)"Driver"
    , 128 // Stack Size
    , NULL
    , 1 // priority
    , NULL );
    
  xTaskCreate(
    TaskBrake
    , (const portCHAR *)"Brakes"
    , 128 // Stack Size
    , NULL
    , 2 // priority
    , NULL );
}

void loop() {
  // Everything done in tasks
}

void TaskDrive(void *pvParameters) {  
  //loop
  while(1){
    if (Serial.available() > 0){
      char transmittedMsg = (char)Serial.read();
      Serial.println(transmittedMsg);
      if (transmittedMsg == 'S') { //Stop
        digitalWrite(enablePin, LOW);
        onStop = true;
        isDriver = true;
        //Serial.println("Stop");
      }
      else if ((transmittedMsg == 'F') && (!forceBreak)) { //Forward
        digitalWrite(enablePin, HIGH);
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        //analogWrite(IN2, 200);
        onStop = false;
        isDriver=false;
        //Serial.println("Forward");
      }
      else if (transmittedMsg == 'R') { //Reverse
        digitalWrite(enablePin, HIGH);
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        onStop = false;
        isDriver=true;
        //Serial.println("Reverse");
      }
    }
    vTaskDelay(50/portTICK_PERIOD_MS); //100ms delay
    
    //Serial.flush();
    Serial.end();
    Serial.begin(9600);
    vTaskDelay(20/portTICK_PERIOD_MS);
  }
}

void TaskBrake(void *pvParameters) {
  //setup pins for distance sensor
  pinMode(powerPin, OUTPUT);
  digitalWrite(powerPin, HIGH);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  //loop
  while(1){
    int distance = getDistance();
    //if (!isDriver) {
      if ((distance < 250) && (distance > 200)){
        forceBreak = true;
        //Serial.println("writing to motors - SLOW");
        
        //slow motor code (if it is not already stopped)
        if ((!onStop) && (!isDriver)) {
          digitalWrite(enablePin, HIGH);
          digitalWrite(IN1, LOW);
          int speedFactor = 255-5*(250-distance);
          //int speedFactor = 0;
          analogWrite(IN2, speedFactor);
        }
      }
      else if (distance <= 200) {
        forceBreak = true;
        //Serial.println("writing to motors - FORCE BREAK");
        if (!isDriver) {
          digitalWrite(enablePin, LOW); // stop motor
        }
      }
      else {
        //Serial.println("not writing to motors - task distance");
        forceBreak = false;
      }
    //}
    //Serial.println(distance);
    vTaskDelay(20/portTICK_PERIOD_MS); //20ms
  }
}

int getDistance(int trigPin, int echoPin) {
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
