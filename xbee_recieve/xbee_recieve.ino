char incoming = 'g';
void setup() 
{
    Serial.begin(9600);
}

void loop() 
{
 
  
   
   if(Serial.available() > 0)
   {
       incoming = Serial.read();
       Serial.println(char(incoming));
       
   }
   else 
   {
      //
   }
   //Serial.write(Serial.read());
}
