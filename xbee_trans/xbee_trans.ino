char incoming = 'A';
void setup() 
{
    Serial.begin(9600);

}

void loop()
{

  Serial.println("Network established");
 // Serial.println(incoming);
  Serial.println("Finally");
  delay(1000);
  /***
  if (Serial.available())
  {
      Serial.println(incoming);
  }
  **/
}
