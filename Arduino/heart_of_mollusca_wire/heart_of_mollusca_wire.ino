#include<Wire.h>
#include <CapacitiveSensor.h>

#define USE_ARDUINO_INTERRUPTS true    // Set-up low-level interrupts for most acurate BPM math.
#include <PulseSensorPlayground.h>     // Includes the PulseSensorPlayground Library.   


#define deviceID 100

//  Variables
const int PulseWire = 0;       // PulseSensor PURPLE WIRE connected to ANALOG PIN 0
int Threshold = 550;           // Determine which Signal to "count as a beat" and which to ignore.
                               // Use the "Gettting Started Project" to fine-tune Threshold Value beyond default setting.
                               // Otherwise leave the default "550" value. 
PulseSensorPlayground pulseSensor;  // Creates an instance of the PulseSensorPlayground object called "pulseSensor"

CapacitiveSensor capSensor = CapacitiveSensor (5,4);
int threshold = 200; //capacity of touch sensor

void setup() {   

  Serial.begin(9600);          // For Serial Monitor

  Wire.begin();

  // Configure the PulseSensor object, by assigning our variables to it. 
  pulseSensor.analogInput(PulseWire);   
  pulseSensor.setThreshold(Threshold);   

  // Double-check the "pulseSensor" object was created and "began" seeing a signal. 
   if (pulseSensor.begin()) {
    Serial.println("We created a pulseSensor Object !");  //This prints one time at Arduino power-up,  or on Arduino reset.  
  }
}

void loop () {
  checkPulse();
}

void checkPulse() {
  //送信する値
  byte value = 0;
  
  // Calls function on our pulseSensor object that returns BPM as an "int".
  // "myBPM" hold this BPM value now. 
  int myBPM = pulseSensor.getBeatsPerMinute();  

  long sensorValue = capSensor.capacitiveSensor(30);
  
  Serial.println((String)"touch:" + sensorValue + " bpm: " + myBPM);
  if (sensorValue > threshold && pulseSensor.sawStartOfBeat()) {
    value = 2;
    delay(10);
  }else if(sensorValue > threshold){
    value = 1; //BPMは無いけど、タッチされている場合
  }
  if (sensorValue == -2) value = 3;

  SendPulse(value);

  delay(50);                    // considered best practice in a simple sketch.

}

void SendPulse (byte value) {
  Wire.beginTransmission(deviceID);
  Wire.write(value);
  Wire.endTransmission();
}

