#include <Wire.h>
#include "I2Cdev.h"
#include "MPU9150Lib.h"
#include "CalLib.h"
#include <dmpKey.h>
#include <dmpmap.h>
#include <inv_mpu.h>
#include <inv_mpu_dmp_motion_driver.h>
#include <EEPROM.h>

#define  DEVICE_TO_USE    0

MPU9150Lib MPU;                                              // the MPU object

//  MPU_UPDATE_RATE defines the rate (in Hz) at which the MPU updates the sensor data and DMP output

#define MPU_UPDATE_RATE  (20)

//  MAG_UPDATE_RATE defines the rate (in Hz) at which the MPU updates the magnetometer data
//  MAG_UPDATE_RATE should be less than or equal to the MPU_UPDATE_RATE

#define MAG_UPDATE_RATE  (10)

//  MPU_MAG_MIX defines the influence that the magnetometer has on the yaw output.
//  The magnetometer itself is quite noisy so some mixing with the gyro yaw can help
//  significantly. Some example values are defined below:

#define  MPU_MAG_MIX_GYRO_ONLY          0                   // just use gyro yaw
#define  MPU_MAG_MIX_MAG_ONLY           1                   // just use magnetometer and no gyro yaw
#define  MPU_MAG_MIX_GYRO_AND_MAG       10                  // a good mix value 
#define  MPU_MAG_MIX_GYRO_AND_SOME_MAG  50                  // mainly gyros with a bit of mag correction 

//  MPU_LPF_RATE is the low pas filter rate and can be between 5 and 188Hz

#define MPU_LPF_RATE   40

//  SERIAL_PORT_SPEED defines the speed to use for the debug serial port

#define  SERIAL_PORT_SPEED  115200

# define motorAfeed A0
# define motorCfeed A1
# define motorGfeed A2
# define motorHfeed A3


struct motors{
  int right;
  int left;
  int pwm;
};
// ----------------------
motors motorD = {
  8, 9, 10};     //motorD : R-8  L-9  PWM-10
//motorD.right = 8;
//motorD.left = 9;
//motorD.pwm = 10;
motors motorC = {
  7,6,5};     //motorC : R-7  L-6  PWM-5
//motorC.right = 7;
//motorC.left = 6;
//motorC.pwm = 5;
motors motorH = {
  2,4,3};     //motorH : R-2  L-4  PWM-3
//motorH.right = 2;
//motorH.left = 4;
//motorH.pwm = 3;
motors motorF = {
  13,12,11};     //motorF : R-13  L-12  PWM-11
//motorF.right = 13;
//motorF.left = 12;
//motorF.pwm = 11;
// ------------------
String message="";
int commaPosition;
String data[7];
int value[7];
int i=0;
boolean rpi_up = false;
void setup ()
{
  Serial.begin(115200);
  pinMode(motorC.right, OUTPUT);
  pinMode(motorC.left, OUTPUT);
  pinMode(motorD.right, OUTPUT);
  pinMode(motorD.left, OUTPUT);
  pinMode(motorH.right, OUTPUT);
  pinMode(motorH.left, OUTPUT);
  pinMode(motorF.right, OUTPUT);
  pinMode(motorF.left, OUTPUT);

  Wire.begin();
  MPU.selectDevice(DEVICE_TO_USE);                        // only really necessary if using device 1
  MPU.init(MPU_UPDATE_RATE, MPU_MAG_MIX_GYRO_AND_MAG, MAG_UPDATE_RATE, MPU_LPF_RATE);   // start the MPU

}
void def()
{
  digitalWrite(motorC.right , LOW);  // MotorC - Right - Pin 7
  digitalWrite(motorC.left , LOW);  // MotorC - Left - Pin 6
  analogWrite(motorC.pwm , LOW);    // MotorC - PWM - Pin 5
  digitalWrite(motorD.right , LOW);    // MotorD - Right - Pin 8
  digitalWrite(motorD.left , LOW);    // MotorD - Left - Pin 9
  analogWrite(motorD.pwm , LOW);    // MotorD - PWM - Pin 10
  //Serial.println("default");

}
void turbo(int pwm)
{
  if(pwm>0)
  {  
    digitalWrite(motorC.right , HIGH);  // MotorC - Right - Pin 7
    digitalWrite(motorC.left , LOW);  // MotorC - Left - Pin 6
    analogWrite(motorC.pwm , pwm);    // MotorC - PWM - Pin 5
    // Serial.println("turbo forward");
  }
  else if (pwm<0)
  {  
    digitalWrite(motorC.right , LOW);  // MotorC - Right - Pin 7
    digitalWrite(motorC.left , HIGH);  // MotorC - Left - Pin 6
    analogWrite(motorC.pwm , -1*pwm);    // MotorC - PWM - Pin 5
    //Serial.println("turbo down");
  }
  else
  {  
    digitalWrite(motorC.right , LOW);  // MotorC - Right - Pin 7
    digitalWrite(motorC.left , LOW);  // MotorC - Left - Pin 6
    analogWrite(motorC.pwm , LOW);    // MotorC - PWM - Pin 5
    // Serial.println("turbo stop");
  }    

}
void move_(int pwm)//pwm= +
{ 
  if(pwm>0){
    digitalWrite(motorD.right , HIGH);  // MotorD - Right - Pin 8
    digitalWrite(motorD.left , LOW);  // MotorD - Left - Pin 9
    analogWrite(motorD.pwm , pwm);  // MotorD - PWM - Pin 10
    //Serial.println(" move forward ");
    //Serial.println(pwm); 
  }
  else if (pwm<0){
    digitalWrite(motorD.right , LOW);  // MotorD - Right - Pin 8
    digitalWrite(motorD.left , HIGH);  // MotorD - Left - Pin 9
    analogWrite(motorD.pwm , -1*pwm);  // MotorD - PWM - Pin 10
    // Serial.println(" move backward ");
    //Serial.println(pwm);
  }
  else {
    digitalWrite(motorD.right , LOW);  // MotorD - Right - Pin 8
    digitalWrite(motorD.left , LOW);  // MotorD - Left - Pin 9
    analogWrite(motorD.pwm , LOW);  // MotorD - PWM - Pin 10
    //Serial.println(" move stop  ");
    //Serial.println(pwm);
  }   
}
void up(int pwm)
{
  if(pwm>0){
    digitalWrite(motorF.right , HIGH);  // MotorF - Right - Pin 12
    digitalWrite(motorF.left , LOW);  // MotorF - Left - Pin 13
    analogWrite(motorF.pwm , pwm);  // MotorF - PWM - Pin 11
    //Serial.println("up ");
    // Serial.println(pwm); 
  }
  else if (pwm<0){
    digitalWrite(motorF.right , LOW);  // MotorF - Right - Pin 12
    digitalWrite(motorF.left , HIGH);  // MotorF - Left - Pin 13
    analogWrite(motorF.pwm , -1*pwm);  // MotorF - PWM - Pin 11
    //Serial.println("down");
    //Serial.println(pwm);
  }
  else {
    digitalWrite(motorF.right , LOW);  // MotorF - Right - Pin 12
    digitalWrite(motorF.left , LOW);  // MotorF - Left - Pin 13
    analogWrite(motorF.pwm , LOW);  // MotorF - PWM - Pin 11
    // Serial.println("stop vertical");
    // Serial.println(pwm);
  }
}


void arm (int x){
  if (x==1) // close
  {  
    digitalWrite(motorH.right , HIGH);  // MotorH - Right - Pin 13
    digitalWrite(motorH.left , LOW);  // MotorH - Left - Pin 12
    analogWrite(motorH.pwm , 255 );  // MotorH - PWM - Pin 11
    // Serial.println("arm close");
    // Serial.println(x); 
  }
  else if (x==-1)//open
  {  
    digitalWrite(motorH.right , LOW);  // MotorH - Right - Pin 13
    digitalWrite(motorH.left , HIGH);  // MotorH - Left - Pin 12
    analogWrite(motorH.pwm , 255 );  // MotorH - PWM - Pin 11
  }
  else 
  {  
    digitalWrite(motorH.right , LOW);  // MotorH - Right - Pin 13
    digitalWrite(motorH.left , LOW);  // MotorH - Left - Pin 12
    analogWrite(motorH.pwm ,LOW );  // MotorH - PWM - Pin 11
  }
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
      // IMU
      MPU.selectDevice(DEVICE_TO_USE);                         // only needed if device has changed since init but good form anyway
      if (MPU.read()) {                                        // get the latest data if ready yet
        MPU.printAngles(MPU.m_fusedEulerPose);                 // print the output of the data fusion
      }


      for (i = 0; i < 5; i++)
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

      if(value[4]==1)
      {
        turbo(value[0]);
        move_(value[0]-value[1]);
      }
      else if (value[4]==0)
      {
        move_(value[0]-value[1]);
        turbo(0);
      }
      up(value[2]);
      arm(value[3]);
      //      if (value[5] == 0)
      //        arm_up_down(value[6]);

    }
  }
  else{
    if (rpi_up)
    {
      MPU.selectDevice(DEVICE_TO_USE);                         // only needed if device has changed since init but good form anyway
      if (MPU.read()) {                                        // get the latest data if ready yet
        MPU.printAngles(MPU.m_fusedEulerPose);                 // print the output of the data fusion
      }
    }
  }
}






