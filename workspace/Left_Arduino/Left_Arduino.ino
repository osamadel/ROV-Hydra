//#include <MemoryFree.h>
#include <Wire.h>
#include "I2Cdev.h"
#include "MPU9150Lib.h"
#include "CalLib.h"
#include <dmpKey.h>
#include <dmpmap.h>
#include <inv_mpu.h>
#include <inv_mpu_dmp_motion_driver.h>
#include <EEPROM.h>

# define pwmC 5 // MotorC:  R->7 L->6 PWM->5
# define pwmD 10 // MotorD: R->8 L->9 PWM->10
# define pwmH 3 // MotorH:  R->2 L->4 PWM->3
# define pwmF 11 // MotorF:  R->13 L->12 PWM->11
# define motorAfeed A0
# define motorCfeed A1
# define motorGfeed A2
# define motorHfeed A3

//  DEVICE_TO_USE selects whether the IMU at address 0x68 (default) or 0x69 is used
//    0 = use the device at 0x68
//    1 = use the device at ox69

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

int commacount = 0;
int errorindex;
String message = "";
int m1,m2,m3,m4;
float v1,v2,v3,v4;
float volt1,volt2,volt3,volt4;
int motor[]={
  7,6,8,9,2,4,13,12};
int commaPosition;
String data[5];
int value[5];
int i=0;
char ch;
boolean valid = false;
char len;
int mlen;
void setup ()
{
  Serial.begin(115200);
  for(int index=0;index<8;index++)
  {
    pinMode(motor[index],OUTPUT);
  }
  //Serial.println("Free Memory = " + String(freeMemory()));
  Wire.begin();
  MPU.selectDevice(DEVICE_TO_USE);                        // only really necessary if using device 1
  MPU.init(MPU_UPDATE_RATE, MPU_MAG_MIX_GYRO_AND_MAG, MAG_UPDATE_RATE, MPU_LPF_RATE);   // start the MPU
  //Serial.println("Free Memory = " + String(freeMemory()));
  while(1)
  {
    Serial.println('1');
    if (Serial.available()){
      char inc = Serial.read();
      if (inc == '1')
        Serial.println("OUT");
      break;
    }
//    digitalWrite(2, 1);
//    digitalWrite(4, 0);
//    analogWrite(3, HIGH);
//    delay(1000);
//    digitalWrite(motor[4],0);
//    digitalWrite(motor[5], 0);
//    analogWrite(pwmH, 255);
  }
  //Serial.println("Free Memory = " + String(freeMemory()));
}
void def()
{
  digitalWrite(motor[0] , LOW);  // MotorC - Right - Pin 7
  digitalWrite(motor[1] , LOW);  // MotorC - Left - Pin 6
  analogWrite(pwmC , LOW);    // MotorC - PWM - Pin 5
  digitalWrite(motor[2] , LOW);    // MotorD - Right - Pin 8
  digitalWrite(motor[3] , LOW);    // MotorD - Left - Pin 9
  analogWrite(pwmD , LOW);    // MotorD - PWM - Pin 10
  //Serial.println("default");

}
void turbo(int pwm)
{
  if(pwm>0)
  {  
    digitalWrite(motor[0] , HIGH);  // MotorC - Right - Pin 7
    digitalWrite(motor[1] , LOW);  // MotorC - Left - Pin 6
    analogWrite(pwmC , pwm);    // MotorC - PWM - Pin 5
    // Serial.println("turbo forward");
  }
  else if (pwm<0)
  {  
    digitalWrite(motor[0] , LOW);  // MotorC - Right - Pin 7
    digitalWrite(motor[1] , HIGH);  // MotorC - Left - Pin 6
    analogWrite(pwmC , -1*pwm);    // MotorC - PWM - Pin 5
    //Serial.println("turbo down");
  }
  else
  {  
    digitalWrite(motor[0] , LOW);  // MotorC - Right - Pin 7
    digitalWrite(motor[1] , LOW);  // MotorC - Left - Pin 6
    analogWrite(pwmC , LOW);    // MotorC - PWM - Pin 5
    // Serial.println("turbo stop");
  }    

}
void move_(int pwm)//pwm= +
{ 
  if(pwm>0){
    digitalWrite(motor[2] , HIGH);  // MotorD - Right - Pin 8
    digitalWrite(motor[3] , LOW);  // MotorD - Left - Pin 9
    analogWrite(pwmD , pwm);  // MotorD - PWM - Pin 10
    //Serial.println(" move forward ");
    //Serial.println(pwm); 
  }
  else if (pwm<0){
    digitalWrite(motor[2] , LOW);  // MotorD - Right - Pin 8
    digitalWrite(motor[3] , HIGH);  // MotorD - Left - Pin 9
    analogWrite(pwmD , -1*pwm);  // MotorD - PWM - Pin 10
    // Serial.println(" move backward ");
    //Serial.println(pwm);
  }
  else {
    digitalWrite(motor[2] , LOW);  // MotorD - Right - Pin 8
    digitalWrite(motor[3] , LOW);  // MotorD - Left - Pin 9
    analogWrite(pwmD , LOW);  // MotorD - PWM - Pin 10
    //Serial.println(" move stop  ");
    //Serial.println(pwm);
  }   
}
void up(int pwm)
{
  if(pwm>0){
    digitalWrite(motor[6] , HIGH);  // MotorF - Right - Pin 12
    digitalWrite(motor[7] , LOW);  // MotorF - Left - Pin 13
    analogWrite(pwmF , pwm);  // MotorF - PWM - Pin 11
    //Serial.println("up ");
    // Serial.println(pwm); 
  }
  else if (pwm<0){
    digitalWrite(motor[6] , LOW);  // MotorF - Right - Pin 12
    digitalWrite(motor[7] , HIGH);  // MotorF - Left - Pin 13
    analogWrite(pwmF , -1*pwm);  // MotorF - PWM - Pin 11
    //Serial.println("down");
    //Serial.println(pwm);
  }
  else {
    digitalWrite(motor[6] , LOW);  // MotorF - Right - Pin 12
    digitalWrite(motor[7] , LOW);  // MotorF - Left - Pin 13
    analogWrite(pwmF , LOW);  // MotorF - PWM - Pin 11
    // Serial.println("stop vertical");
    // Serial.println(pwm);
  }
}



void arm (int x){
  if (x==1) // close
  { 
    
    digitalWrite(motor[4] , HIGH);  // MotorH - Right - Pin 4
    digitalWrite(motor[5] , LOW);  // MotorH - Left - Pin 5
    analogWrite(pwmH , 125);  // MotorH - PWM - Pin 3
    
    // Serial.println("arm close");
    // Serial.println(x); 
  }
  else if (x==-1)//open
  {  
    digitalWrite(motor[4] , LOW);  // MotorH - Right - Pin 4
    digitalWrite(motor[5] , HIGH);  // MotorH - Left - Pin 5
    analogWrite(pwmH , 125);  // MotorH - PWM - Pin 3
    //Serial.println("arm open ");
    // Serial.println(x); 
  }
  else 
  {  
    digitalWrite(motor[4] , LOW);  // MotorH - Right - Pin 4
    digitalWrite(motor[5] , LOW);  // MotorH - Left - Pin 5
    analogWrite(pwmH ,LOW );  // MotorH - PWM - Pin 3
    //Serial.println("stop arm ");
    //Serial.println(x);
  }

}
void loop()
{
  MPU.selectDevice(DEVICE_TO_USE);                         // only needed if device has changed since init but good form anyway
  if (MPU.read()) {                  // get the latest data if ready yet
    //Serial.print('*');
//  MPU.printQuaternion(MPU.m_rawQuaternion);              // print the raw quaternion from the dmp
//  MPU.printVector(MPU.m_rawMag);                         // print the raw mag data
//  MPU.printVector(MPU.m_rawAccel);                       // print the raw accel data
//  MPU.printAngles(MPU.m_dmpEulerPose);                   // the Euler angles from the dmp quaternion
//  MPU.printVector(MPU.m_calAccel);                       // print the calibrated accel data
//  MPU.printVector(MPU.m_calMag);                         // print the calibrated mag data
    Serial.print('*');
    MPU.printAngles(MPU.m_fusedEulerPose);                 // print the output of the data fusion
    Serial.println('$');
  }
  if(Serial.available() > 0)
  {
    while (!valid)
    {
      //Serial.println("No enough commas. Comma Count = " + String(commacount));
      //commacount = 0;
      ch = Serial.read();
      if (ch == '*')
      {
        message= Serial.readStringUntil('$');  // Found '$'
        len = message.charAt(0);
        mlen = message.length();
        message = message.substring(1,mlen);
        mlen--;
        errorindex = message.indexOf('*');
        while(errorindex >= 0)
        {
          //commacount = 0;
          message = message.substring(errorindex+1,message.length());
          //Serial.println("Found anomaly " + String(errorindex));
          errorindex = message.indexOf('*');

        }
        /*for (int i = 0; i < message.length(); i++)
         {
         if (message.charAt(i) == ',')
         commacount++;
         }*/
      }
      //      Serial.print("Len= ");
      //      Serial.println(len,BIN);
      //      Serial.print("message.length= ");
      //      Serial.println(mlen);
      //      Serial.println(message);
      if (mlen == len)
      {
        valid = true;
      }
    }
    valid = false;
    //commacount = 0;
    //    Serial.println('*' + message + '$');
    for(i=0;i<5;i++)
    {
      commaPosition = message.indexOf(',');
      if(commaPosition != -1)
      {
        data[i] =message.substring(0,commaPosition); 
        value[i] = data[i].toInt();
        message = message.substring(commaPosition+1, message.length());
      }
      else if(commaPosition == -1 && message.length() > 0){ 
        data[i] = message;
        value[i] = data[i].toInt();
      }
    }

    if(value[4]==1)
    {
      turbo(value[0]);
      move_(value[0]+value[1]);
    }
    else if (value[4]==0)
    {
      move_(value[0]+value[1]);
      turbo(0);
    }
    up(value[2]);
    arm(value[3]);

    /////// feed back ////
    /*m1=analogRead(motorAfeed);
     m2=analogRead(motorCfeed);
     m3=analogRead(motorGfeed);
     m4=analogRead(motorHfeed);
     v1=m1*(5.0/1023.0);
     v2=m2*(5.0/1023.0);
     v3=m3*(5.0/1023.0);
     v4=m4*(5.0/1023.0);
     volt1=((50/9)*v1)-12;
     volt2=((50/9)*v2)-12;
     volt3=((50/9)*v3)-12;
     volt4=((50/9)*v4)-12;
     ////print data //// 
     Serial.print("/*");
     Serial.print(volt1);
     Serial.print(",");
     Serial.print(volt2);
     Serial.print(",");
     Serial.print(volt3);
     Serial.print(",");
     Serial.print(volt4);
    /*Serial.print(",");
     Serial.print(pitch);
     Serial.print(",");
     Serial.print(roll);
     Serial.print(",");
     Serial.print(yaw);
     Serial.println("\n");
     */
  }
  Serial.flush();
  message = "";
}




