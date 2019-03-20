void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.print(GetTransmittedMessage());
  delay(100);
}

String GetTransmittedMessage()
{
  String transmittedMsg = "";
  
  while (!transmittedMsg.endsWith("*"))
  {
    if (Serial.available() > 0)
    {
      transmittedMsg += (char)Serial.read();
    }
  }

  return transmittedMsg;
}
