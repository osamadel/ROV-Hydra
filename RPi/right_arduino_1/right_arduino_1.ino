# define motorAfeed A0
# define motorCfeed A1
# define motorGfeed A2
# define motorHfeed A3
#define pump 6
#include <Servo.h>
Servo pwmG;
struct motors{
  int right;
  int left;
  int pwm;
};
struct motor{
  int right;
  int left; 
};
// ----------------------
// Motors on this arduino :
motors motorE = {
  2, 4, 3};  // Turbo
motors arm1 = {
  13, 12, 11};  // Open close arm    
motor arm2 = {
  9, 10};   // Rotation
motor arm3 = {
  8, 7};    // Bind
// Brushless
// ------------------
String message="";
int commaPosition;
String data[7];
int value[7];
int i=0;
boolean rpi_up = false;
boolean one_msg = false;
// FOR DEBUGGING
String lastMessage;
void setup ()
{
  Serial.begin(115200);
  pinMode(motorE.right, OUTPUT);
  pinMode(motorE.left, OUTPUT);

  pinMode(arm1.right, OUTPUT);
  pinMode(arm1.left, OUTPUT);

  pinMode(arm2.right, OUTPUT);
  pinMode(arm2.left, OUTPUT);

  pinMode(arm3.right, OUTPUT);
  pinMode(arm3.left, OUTPUT);

  pinMode(pump,OUTPUT);
  pwmG.attach(5);    // Brushless
}

void arm_open_close(int x )
{

  if(x == 1)  //open
  {
    digitalWrite(arm1.right,HIGH);
    digitalWrite(arm1.left,LOW);
    //analogWrite(arm1.pwm,255);
    Serial.println("arm open ");
  }
  else if (x == -1) //close
  {
    digitalWrite(arm1.right,LOW);
    digitalWrite(arm1.left,HIGH);
  //  analogWrite(arm1.pwm,255);
    Serial.println("arm close");
  }
  else
  {
    digitalWrite(arm1.right,LOW);
    digitalWrite(arm1.left,LOW);
//    analogWrite(arm1.pwm,LOW);
    // Serial.println("arm stop ");
  }
}

void arm_bind(int x)
{
  if(x == -1) // arm move up
  {
    digitalWrite(arm3.right, HIGH);
    digitalWrite(arm3.left, LOW);
    //analogWrite(arm3.pwm, 255);
    Serial.println("arm move up");
  } 
  else if (x == 1) // arm move down
  {
    digitalWrite(arm3.right,LOW);
    digitalWrite(arm3.left,HIGH);
//    analogWrite(arm3.pwm,255);
    Serial.println("arm move down");
  }
  else
  {
    digitalWrite(arm3.right,LOW);
    digitalWrite(arm3.left,LOW);
   // analogWrite(arm3.pwm,LOW);
    // Serial.println("arm stop");
  }
}
void arm_spin(int x)
{
  if(x == -1) // spin right 
  {
    digitalWrite(arm2.right,HIGH);
    digitalWrite(arm2.left,LOW);
    //analogWrite(arm2.pwm,255);
    Serial.println("arm spin right ");
  }
  else if (x == 1) //spin left 
  {
    digitalWrite(arm2.right,LOW);
    digitalWrite(arm2.left,HIGH);
    //analogWrite(arm2.pwm,255);
    Serial.println("arm spin left ");
  }
  else
  {
    digitalWrite(arm2.right,LOW);
    digitalWrite(arm2.left,LOW);
    //analogWrite(arm2.pwm,LOW);
//    Serial.println("arm stop ");
  }

}
void pumping (int x )
{
  if (x==1)
  {
    digitalWrite(pump,HIGH);
    //Serial.println("pumping");
  }
  else 
  { 
    digitalWrite(pump,LOW);
    //Serial.println("stop pumping");
  }

}
void up(int pwm)
{
  pwmG.writeMicroseconds(pwm);
}
void turbo(int pwm)
{
  if(pwm>0){
    digitalWrite(motorE.right , HIGH);  
    digitalWrite(motorE.left , LOW);  
    analogWrite(motorE.pwm , pwm);  
    //Serial.println("turbo forward ");
    // Serial.println(pwm); 
  }
  else if (pwm<0){
    digitalWrite(motorE.right , LOW);  
    digitalWrite(motorE.left , HIGH);  
    analogWrite(motorE.pwm , -1*pwm);  
    //Serial.("turbo reversr");
    //Serial.println(pwm);
  }
  else {
    digitalWrite(motorE.right , LOW);  
    digitalWrite(motorE.left , LOW);  
    analogWrite(motorE.pwm , LOW);  
    // Serial.println("stop turbo");
    // Serial.println(pwm);
  }

}

void loop()
{
  if (Serial.available() > 0)
  {
    char msg = Serial.read();
    if (msg == '2')
    {
      Serial.println("RIGHT");
      rpi_up = true;
    }
    else if (msg == '*')
    {
      message = Serial.readStringUntil('$');
      lastMessage = message;
      Serial.println(message);
      one_msg = true;

      for (i = 0; i < 7; i++)
      { 
        commaPosition = message.indexOf(',');
        if (commaPosition != -1 && commaPosition >= 0)
        { 
          data[i] = message.substring(0, commaPosition);
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


      // right_left(value[0]);
      // SpeedY, Turbo, Zspeed, open_close_arm, arm mode(1=spin, 0=bind), bind or spin, pump
      // *0, 0, 1500, 0,1,1,0$
//      *0, 0, 1500, 0,1,0,0$
      // *255, 1, 1400, 0,0,-1,0$
      if(value[1]==1)
      { 
        turbo(value[0]);
      }
      else if (value[1]==0)
      { 
        turbo(0);
      }
      up(value[2]);
      if ( value[3] != 0 || value[5] != 0 )
      {
        analogWrite(arm1.pwm,255);
      }
      else if(value[3] == 0 && value[5] == 0 )
      {
        analogWrite(arm1.pwm,LOW);
      }
      arm_open_close(value[3]);
      if(value[4]==1)
      {
        arm_spin(value[5]);
        arm_bind(0);
      }
      else if(value[4]==0)
      {
        arm_spin(0);
        arm_bind(value[5]);
      }
      pumping(value[6]);


    }
  }
  else{
    if (rpi_up && one_msg)
    {
      Serial.println(lastMessage);  
    }
  }
}






