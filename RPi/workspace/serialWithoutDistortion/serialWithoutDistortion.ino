// Adding the prineln() to send the sensors readings
// must be put in else{} statement after the if(ping=='2').
// because otherwise, arduino will send the sensors readings
// infinitely and will cause delays and problems to the
// python code which is waiting for '0' as a response.
String msg;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.setTimeout(100);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0)
  {
    char ping = Serial.read();
    if (ping == '2')
    {
      Serial.println('2');
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
