#include <Servo.h>
#define arm1_right 13
#define arm1_left 12
Servo pwmF ;
struct motors{
  int right;
  int left;
  int pwm;
};
motors motorD;     //motorD : R-8  L-9  PWM-10
motors motorC;     //motorC : R-7  L-6  PWM-5
motors motorH;     //motorH : R-2  L-4  PWM-3
String message="";
int commaPosition;
String data[7];
int value[7];
int i=0;
void turbo(int pwm)
{ if(pwm>0)
   {digitalWrite(motorC.right,HIGH);
    digitalWrite(motorC.left,LOW);
    analogWrite(motorC.pwm,pwm);
//    Serial.println("turbo forward");
    }
   else if (pwm<0)
    {digitalWrite(motorC.right,LOW);
     digitalWrite(motorC.left,HIGH);
     analogWrite(motorC.pwm,-1*pwm);
//     Serial.println("turbo reverse");
     }
    else
     {digitalWrite(motorC.right,LOW);
      digitalWrite(motorC.left,LOW);
      analogWrite(motorC.pwm,LOW);
      //Serial.println("turbo stop");
      }
}
void move_(int pwm) // pwm= +
{if(pwm>0)
   {digitalWrite(motorD.right,HIGH);
    digitalWrite(motorD.left,LOW);
    analogWrite(motorD.pwm,pwm);
//    Serial.println("move forward");
    }
   else if (pwm<0)
    {digitalWrite(motorD.right,LOW);
     digitalWrite(motorD.left,HIGH);
     analogWrite(motorD.pwm,-1*pwm);
 //    Serial.println("move reverse");
     }
    else
     {digitalWrite(motorD.right,LOW);
      digitalWrite(motorD.left,LOW);
      analogWrite(motorD.pwm,LOW);
      //Serial.println("move stop");
      }
}
void up(int pwm )
{
  pwmF.writeMicroseconds(pwm);
  //standard servos a parameter value
  //stop = 1500
  // Max. Forward = 2000
  //Max. Backward = 1000  
 // Serial.print("brushless move  ");
 // Serial.println(pwm);
}
void arm_open_close(int x )
{
   
  if(x == 1)  //open
   {digitalWrite(motorH.right,HIGH);
    digitalWrite(motorH.left,LOW);
  //  analogWrite(motorH.pwm,255);
   // Serial.println("arm open ");
    }
   else if (x == -1) //close
    {digitalWrite(motorH.right,LOW);
     digitalWrite(motorH.left,HIGH);
    // analogWrite(motorH.pwm,255);
    // Serial.println("arm close");
     }
    else
     {digitalWrite(motorH.right,LOW);
      digitalWrite(motorH.left,LOW);
      //analogWrite(motorH.pwm,LOW);
     // Serial.println("arm stop ");
      }
    }
  void arm_spin(int x)
  {
  if(x == -1) // spin right 
   {
    digitalWrite(arm1_right,HIGH);
    digitalWrite(arm1_left,LOW);
    //analogWrite(motorH.pwm,255);
   // Serial.println("arm spin right ");
    }
   else if (x == 1) //spin left 
    {digitalWrite(arm1_right,LOW);
     digitalWrite(arm1_left,HIGH);
     //analogWrite(motorH.pwm,255);
    // Serial.println("arm spin left ");
     }
    else
     {digitalWrite(arm1_right,LOW);
      digitalWrite(arm1_left,LOW);
      //analogWrite(motorH.pwm,LOW);
     // Serial.println("arm stop ");
      }

}
void setup()
{ 
 motorD.right=8;
 motorD.left=9;
 motorD.pwm=10;
 motorC.right=7;
 motorC.left=6;
 motorC.pwm=5;
 motorH.right=2;
 motorH.left=4;
 motorH.pwm=3;
 pwmF.attach(11);
 pinMode(arm1_right,OUTPUT );
 pinMode(arm1_left,OUTPUT );
 analogWrite(motorH.pwm,255); 
  Serial.begin(115200);
  while (!Serial);
  if(Serial.available()>0)
  {   char ping = Serial.read();
      if(ping=='2') 
        Serial.println("1");     // left arduino   
  }
 }
void loop()
{
 
 if(Serial.available()>0)
 {
  char msg = Serial.read();
  if(msg=='2')
         Serial.println("1");        
  else if(msg=='*')
       {
          message = Serial.readStringUntil('$');
        //    Serial.println(message);
         
          
for(i=0;i<7;i++)
{ commaPosition = message.indexOf(',');
  if(commaPosition != -1 && commaPosition >=0)
    { data[i] =message.substring(0,commaPosition); 
      value[i] = data[i].toInt();
      message = message.substring(commaPosition+1, message.length());
     }
  else if(commaPosition == -1 && message.length() > 0)
   { 
     if (message != "")
       data[i] = message;
    value[i] = data[i].toInt();
   } 
}
  
 if(value[4]==1)
   {  turbo(value[0]);
      move_(value[0]+value[1]);
   }
 else if (value[4]==0)
   {  move_(value[0]+value[1]);
      turbo(0);
    }
   
  up(value[2]);
  arm_open_close(value[3]);   
 if(value[6]==1)  
   arm_spin(value[5]);

 
    }
 }
 }
 
        
   
  

