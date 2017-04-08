#include <Servo.h>
#define arm1_right 13
#define arm1_left 12
boolean rpi_up = false;
Servo pwmG ;
struct motors {
  int right;
  int left;
  int pwm;
};
motors motorA;     //motorA : R-8  L-9  PWM-10
motors motorB;     //motorB : R-7  L-6  PWM-5
motors motorE;     //motorE : R-2  L-4  PWM-3
String message = "";
int commaPosition;
String data[7];
int value[7];
int i = 0;
void turbo(int pwm)
{ if (pwm > 0)
  { digitalWrite(motorB.right, HIGH);
    digitalWrite(motorB.left, LOW);
    analogWrite(motorB.pwm, pwm);
    //Serial.println("turbo forward");
  }
  else if (pwm < 0)
  { digitalWrite(motorB.right, LOW);
    digitalWrite(motorB.left, HIGH);
    analogWrite(motorB.pwm, -1 * pwm);
    //Serial.println("turbo reverse");
  }
  else
  { digitalWrite(motorB.right, LOW);
    digitalWrite(motorB.left, LOW);
    analogWrite(motorB.pwm, LOW);
    //Serial.println("turbo stop");
  }
}
void move_(int pwm) // pwm= -
{ if (pwm > 0)
  { digitalWrite(motorA.right, HIGH);
    digitalWrite(motorA.left, LOW);
    analogWrite(motorA.pwm, pwm);
    //Serial.println("move forward");
  }
  else if (pwm < 0)
  { digitalWrite(motorA.right, LOW);
    digitalWrite(motorA.left, HIGH);
    analogWrite(motorA.pwm, -1 * pwm);
    //Serial.println("move reverse");
  }
  else
  { digitalWrite(motorA.right, LOW);
    digitalWrite(motorA.left, LOW);
    analogWrite(motorA.pwm, LOW);
    //Serial.println("move stop");
  }
}
void up(int pwm )
{
  pwmG.writeMicroseconds(pwm);
  //standard servos a parameter value
  //stop = 1500
  // Max. Forward = 2000
  //Max. Backward = 1000
  //  Serial.print("brushless move  ");
  //  Serial.println(pwm);
}
void right_left(int pwm)
{ if (pwm > 0)
  { digitalWrite(motorE.right, HIGH);
    digitalWrite(motorE.left, LOW);
    analogWrite(motorE.pwm, pwm);
    //  Serial.println("right");
  }
  else if (pwm < 0)
  { digitalWrite(motorE.right, LOW);
    digitalWrite(motorE.left, HIGH);
    analogWrite(motorE.pwm, -1 * pwm);
    // Serial.println("left");
  }
  else
  { digitalWrite(motorE.right, LOW);
    digitalWrite(motorE.left, LOW);
    analogWrite(motorE.pwm, LOW);
    // Serial.println("stop horizontal R/L");
  }
}
void arm_up_down(int x)
{
  if (x == -1 ) // arm move up
  {
    digitalWrite(arm1_right, HIGH);
    digitalWrite(arm1_left, LOW);
    //   Serial.println("arm move up");
  }
  else if (x == 1) // arm move down
  {
    digitalWrite(arm1_right, LOW);
    digitalWrite(arm1_left, HIGH);
    // Serial.println("arm move down");
  }
  else
  {
    digitalWrite(arm1_right, LOW);
    digitalWrite(arm1_left, LOW);
    //   Serial.println("arm stop");
  }
}
void setup()
{
  motorA.right = 8;
  motorA.left = 9;
  motorA.pwm = 10;
  motorB.right = 7;
  motorB.left = 6;
  motorB.pwm = 5;
  motorE.right = 2;
  motorE.left = 4;
  motorE.pwm = 3;
  pwmG.attach(11);
  pinMode(arm1_right, OUTPUT );
  pinMode(arm1_left, OUTPUT );
  Serial.begin(115200);
  Serial.setTimeout(100);
//  while (!Serial);
//  if (Serial.available() > 0)
//  { char ping = Serial.read();
//    if (ping == '2')
//      Serial.println("2");     // right arduino
//  }
}
void loop()
{

  if (Serial.available() > 0)
  {
    char msg = Serial.read();
    if (msg == '2')
      {
        Serial.println("2");
        rpi_up = true;
      }
    else if (msg == '*')
    {
      message = Serial.readStringUntil('$');
      Serial.println(rpi_up);


      for (i = 0; i < 7; i++)
      { commaPosition = message.indexOf(',');
        if (commaPosition != -1 && commaPosition >= 0)
        { data[i] = message.substring(0, commaPosition);
          value[i] = data[i].toInt();
          message = message.substring(commaPosition + 1, message.length());
        }
        else if (commaPosition == -1 && message.length() > 0)
        {
          if (message != "")
            data[i] = message;
          value[i] = data[i].toInt();
        }
      }

      if (value[4] == 1)
      { turbo(value[0]);
        move_(value[0] - value[1]);
      }
      else if (value[4] == 0)
      { move_(value[0] - value[1]);
        turbo(0);
      }
      up(value[2]);
      right_left(value[3]);
      if (value[5] == 0)
        arm_up_down(value[6]);

    }
    }else{
      if (rpi_up)
      {
        Serial.println("Sensors in else");
      }
  }
}





