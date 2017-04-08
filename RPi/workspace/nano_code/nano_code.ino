// ****** HYDROBOTS 2015 ******
// **** Arduino Nano Code. ****
// Written By: OSAMA ADEL SHOKRY.
// Receives servo angles and Arm bend and rotation signals and launch these actuators.
// Collect sensor readings and send them simultaneously to the RPi.

// ** DIGITAL OUTPUTS **
// Right Servo --> D5
// Left Servo --> D9
// Fixed Arm PWM --> D3
// M1(R) --> D8
// M1(L) --> D7
// M2(R) --> D6
// M2(L) --> D4

// ** ANALOG INPUTS **
// A0 --> Leakage
// A1 --> Temperature Left
// A2 --> Current Left
// A3 --> Voltage Left
// A4 --> Temperature Right
// A5 --> Current Right
// A6 --> Voltage Right
// A7 --> Input Voltage from tether.

// Includes
#include<Servo.h>

// Defines
#define armM1R 8
#define armM1L 7
#define armM2R 6
#define armM2L 4
#define fixedPWM 3
#define rightservo 5
#define leftservo 9
#define leakage A0
#define tempL A1
#define currL A2
#define voltL A3
#define tempR A4
#define currR A5
#define voltR A6
#define inputV A7

// Declaration of variables.
Servo rservo;
Servo lservo;
int TempLeft, CurrLeft, VoltLeft, TempRight, CurrRight, VoltRight, InputVolt, Leak;
// Message verification variables
int errorindex, commaPosition, mlen, i=0;
int value[3] = {0,0,90};
String message = "";
String data[3];
char ch;
char len;
boolean valid = false;


// Setup function.
void setup()
{
  // Initiate serial communication.
  // This device should be called /dev/ttyAMA0 on RPi.
  Serial.begin(115200);

  // Set pins to OUTPUTs.
  pinMode(armM1R, OUTPUT);
  pinMode(armM1L, OUTPUT);
  pinMode(armM2R, OUTPUT);
  pinMode(armM2L, OUTPUT);
//  pinMode(fixedPWM, OUTPUT);
//  pinMode(rightservo, OUTPUT);
//  pinMode(leftservo, OUTPUT);
  // Set sensors pins to INPUTs.
  pinMode(leakage, INPUT);
  pinMode(tempL, INPUT);
  pinMode(currL, INPUT);
  pinMode(voltL, INPUT);
  pinMode(tempR, INPUT);
  pinMode(currR, INPUT);
  pinMode(voltL, INPUT);
  pinMode(inputV, INPUT);

  // Attaching servoes to their pins.
  rservo.attach(rightservo);
  lservo.attach(leftservo);
//  delay(50);
  


}

// M1 function.
void M1 (int x){
  if (x==1) // close
  {  
    digitalWrite(armM1R , HIGH);  // MotorH - Right - Pin 4
    digitalWrite(armM1L , LOW);  // MotorH - Left - Pin 5
    analogWrite(fixedPWM , 125 );  // MotorH - PWM - Pin 3
    // Serial.println("arm close");
    // Serial.println(x); 
  }
  else if (x==-1)//open
  {  
    digitalWrite(armM1R , LOW);  // MotorH - Right - Pin 4
    digitalWrite(armM1L , HIGH);  // MotorH - Left - Pin 5
    analogWrite(fixedPWM , 125 );  // MotorH - PWM - Pin 3
    //Serial.println("arm open ");
    // Serial.println(x); 
  }
  else 
  {  
    digitalWrite(armM1R , LOW);  // MotorH - Right - Pin 4
    digitalWrite(armM1L , LOW);  // MotorH - Left - Pin 5
    analogWrite(fixedPWM ,LOW );  // MotorH - PWM - Pin 3
    //Serial.println("stop arm ");
    //Serial.println(x);
  }
}

// M2 Function
void M2 (int x){
  if (x==1) // close
  {  
    digitalWrite(armM2R , HIGH);  // MotorH - Right - Pin 4
    digitalWrite(armM2L , LOW);  // MotorH - Left - Pin 5
    analogWrite(fixedPWM , 255 );  // MotorH - PWM - Pin 3
    // Serial.println("arm close");
    // Serial.println(x); 
  }
  else if (x==-1)//open
  {  
    digitalWrite(armM2R , LOW);  // MotorH - Right - Pin 4
    digitalWrite(armM2L , HIGH);  // MotorH - Left - Pin 5
    analogWrite(fixedPWM , 255 );  // MotorH - PWM - Pin 3
    //Serial.println("arm open ");
    // Serial.println(x); 
  }
  else 
  {  
    digitalWrite(armM2R , LOW);  // MotorH - Right - Pin 4
    digitalWrite(armM2L , LOW);  // MotorH - Left - Pin 5
    analogWrite(fixedPWM ,LOW );  // MotorH - PWM - Pin 3
    //Serial.println("stop arm ");
    //Serial.println(x);
  }
}

// Loop function.
void loop()
{
  if(Serial.available() > 0)  // Buffer is not empty.
  {
    while (!valid)  // As long a message is not validated.
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
        //Serial.println("Message: " + message);
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
      // Serial.print("Len= ");
      // Serial.println(len,BIN);
      // Serial.print("message.length= ");
      // Serial.println(mlen);
      // Serial.println(message);
      if (mlen == len)
      {
        valid = true;
      }
    }
    valid = false;
    // Serial.println('*' + message + '$');
    for(i=0;i<3;i++)
    {
      commaPosition = message.indexOf(',');
      if(commaPosition != -1)
      {
        data[i] =message.substring(0,commaPosition); 
        value[i] = data[i].toInt();
        message = message.substring(commaPosition+1, message.length());
      }
      else if(commaPosition == -1 && message.length() > 0){ 
        if (message != "")
          data[i] = message;
        value[i] = data[i].toInt();
      }
    }

    // Assign the values to the motors.
    M1(value[0]);
    M2(value[1]);
    rservo.write(180-value[2]);
    lservo.write(value[2]);
    delay(15);

    // Collect sensors


  }
  TempLeft = analogRead(tempL);
  CurrLeft = analogRead(currL);
  VoltLeft = analogRead(voltL);
  Leak = analogRead(leakage);
  TempRight = analogRead(tempR);
  CurrRight = analogRead(currR);
  VoltRight = analogRead(voltR);
  InputVolt = analogRead(inputV);
  String sensors = '*' + String(InputVolt) + ',' +
    String(Leak) + ',' +
    String(TempLeft) + ',' +
    String(CurrLeft) + ',' +
    String(VoltLeft) + ',' +
    String(TempRight) + ',' +
    String(CurrRight) + ',' +
    String(VoltRight) + '$';
  Serial.println(sensors);
}


