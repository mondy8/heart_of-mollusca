#include <Wire.h>

//Wire settings
#define deviceId 100
byte recievedValue = 0;
int deadCount = 0;

//---------pulse sensor------------
//unsigned long delayTime;
//  Variables
#define fadePin 6                  // pin to do fancy classy fading blink at each beat
int fadeRate = 0;                 // used to fade LED on with PWM on fadePin

//---------pump motor valve ------------
#define pumpEnable01  9 //analogWriteなのでPWM
#define pumpEnable02  10 //analogWriteなのでPWM
#define pumpMotor01   8 
#define pumpMotor02   7
#define valve1        11  //pump01      "HIGH == open" "LOW == closed"
#define valve2        12  //pump02

//ポンプの動作状態
typedef enum{
  PS_AIR_IN,
  PS_AIR_OUT
}PumpState;
PumpState pumpState;

//typedef enum{
//  VALVE_CLOSE,
//  VALVE_OPEN,
//  DEAD
//}PumpState02;
//PumpState02 pumpState02;

void setup(){
  pinMode(fadePin,OUTPUT);          // pin that will fade to your heartbeat!
  Serial.begin(9600);             // we agree to talk fast!

  //-----Wire setings---------
  Wire.begin(deviceId);
  Wire.onReceive(ReceivePulse);

  //-----↓pump setting---------
  pinMode(pumpEnable01,OUTPUT);
  pinMode(pumpMotor01,OUTPUT);
  pinMode(pumpEnable02,OUTPUT);
  pinMode(pumpMotor02,OUTPUT);
  
  pinMode(valve1,OUTPUT);
  pinMode(valve2,OUTPUT);
  
  pumpState = PS_AIR_OUT;
}

//  Where the Magic Happens
void loop(){
  check ();
  motion();
  delay(30);
}

void check () {

    if(recievedValue == 0) {
       pumpState = PS_AIR_OUT;
       deadCount++;
    }
    if(recievedValue == 1) {
    }

    if(recievedValue == 2) {//心拍感じたとき
      fadeRate = 255;
      if(pumpState == PS_AIR_OUT){
       pumpState = PS_AIR_IN;
      }else{
       pumpState = PS_AIR_OUT;
      }
      delay(100);
    }

    if(recievedValue == 3) {
      fadeRate = 255;
    }

/*   脈打つタイプ
    if(recievedValue == 0) {
       pumpState02 = DEAD;
    }
    if(recievedValue == 1) {
      analogWrite(fadePin, 100);
      pumpState02 = VALVE_OPEN;
    }

    if(recievedValue == 2) {//心拍感じたとき
      fadeRate = 255;
      pumpState02 = VALVE_CLOSE;
    }
    */

    if(deadCount>120){
      fadeRate = 255;
      deadCount = 0;
    }
}

void motion() {
  
  
  //pump motion
  pumpSafety();
  
  switch (pumpState) {
    
    case PS_AIR_IN:
      digitalWrite(pumpMotor01, HIGH);
      analogWrite(pumpEnable01, 140); //Max 255
      digitalWrite(valve1,LOW);
      
      digitalWrite(pumpMotor02, HIGH);
      analogWrite(pumpEnable02, 170); //Max 255
      digitalWrite(valve2,LOW);
    break;
    
    default:
    case PS_AIR_OUT:
      digitalWrite(pumpMotor01, LOW);
      analogWrite(pumpEnable01, 0); //Max 255
      digitalWrite(valve1,HIGH);
      digitalWrite(pumpMotor02, LOW);
      analogWrite(pumpEnable02, 0); //Max 255
      digitalWrite(valve2,HIGH);
    break;
  }

    fadeRate -= 15;                         //  set LED fade value
    fadeRate = constrain(fadeRate,0,255);   //  keep LED fade value from going into negative numbers!
    analogWrite(fadePin,fadeRate);          //  fade LED
}

//ポンプの安全を守る
void pumpSafety () {
  int pressure01 = analogRead(A1);
  int pressure02 = analogRead(A2);
  //Serial.println((String)"pressure: " + pressure01 + ", " + pressure02);
  
  if(pressure01 > 270 || pressure02 > 240 ) {
      fadeRate = 255;
      digitalWrite(valve1,HIGH);
      digitalWrite(pumpMotor01, LOW);
      analogWrite(pumpEnable01, 0);
      
      digitalWrite(valve2,HIGH);
      digitalWrite(pumpMotor02, LOW);
      analogWrite(pumpEnable02, 0);
      
      delay(1000); 
    }
}

void ReceivePulse (int numBytes) {
  byte value = Wire.read();
  Serial.println((String)"Receive " + value + (String)" : PumpState " + pumpState );
  recievedValue = value;
}

