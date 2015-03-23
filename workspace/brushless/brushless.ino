#include<Servo.h>
Servo s;
int x;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.setTimeout(100);
  s.attach(9);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0)
  {
     x = String(Serial.readStringUntil('\n')).toInt();
     Serial.println(x);
     s.writeMicroseconds(x);
//     analogWrite(x, 9);
  }
}
