String msg;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200z);
  Serial.setTimeout(100);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0)
  {
    char ping = Serial.read();
    if (ping == '2')
    {
      Serial.println('0');
    }else if(ping == '*')
    {
      msg = Serial.readStringUntil('$');
      Serial.println(msg);
//      Serial.println("Sensors Readings");
//      Serial.flush();
    }
  }
  
//  delay(100);
}
