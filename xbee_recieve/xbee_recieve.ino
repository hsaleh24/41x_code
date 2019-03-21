#include <SoftwareSerial.h>
char incoming = 'g';

//SoftwareSerial Serial1;

void setup() 
{
    Serial.begin(9600);
    Serial1.begin(9600);
}

void loop() 
{
 
  
   
   if(Serial1.available() > 0)
   {
       incoming = Serial1.read();
       Serial.println(char(incoming));

   }
   else 
   {
      //
   }
   //Serial.write(Serial.read());
}
