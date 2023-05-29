/*
 * Created by ArduinoGetStarted.com
 *
 * This example code is in the public domain
 *
 * Tutorial page: https://arduinogetstarted.com/tutorials/arduino-touch-sensor-piezo-buzzer
 */

const int TOUCH_SENSOR_PIN = 7;  // Arduino pin connected to touch sensor's pin
const int BUZZER_PIN = 3;        // Arduino pin connected to Buzzer's pin

#define USE_ARDUINO_INTERRUPTS true  // Set-up low-level interrupts for most acurate BPM math
#include <PulseSensorPlayground.h>   // Includes the PulseSensorPlayground Library

const int PulseWire = 0;  // 'S' Signal pin connected to A0
const int LED13 = 13;     // The on-board Arduino LED
int Threshold = 550;      // Determine which Signal to "count as a beat" and which to ignore

PulseSensorPlayground pulseSensor;  // Creates an object

void setup() {
  Serial.begin(9600);  // initialize serial
  pulseSensor.analogInput(PulseWire);
  pulseSensor.setThreshold(Threshold);

  // Double-check the "pulseSensor" object was created and began seeing a signal
  if (pulseSensor.begin()) {
    Serial.println("PulseSensor object created!");
  }
  pinMode(TOUCH_SENSOR_PIN, INPUT);  // set arduino pin to input mode
  pinMode(BUZZER_PIN, OUTPUT);       // set arduino pin to output mode
}

void loop() {
  int touchState = digitalRead(TOUCH_SENSOR_PIN);  // read new state

  int myBPM = pulseSensor.getBeatsPerMinute();  // Calculates BPM



  delay(2000);

  if (touchState == HIGH) {
    Serial.println("The sensor is being touched");
    if (pulseSensor.sawStartOfBeat()) {              // Constantly test to see if a beat happened
      Serial.println("♥  A HeartBeat Happened ! ");  // If true, print a message
      Serial.print("BPM: ");
      Serial.println(myBPM); 
      if(myBPM < 60){
        Serial.println(" UR DROWSY BRO");
        digitalWrite(BUZZER_PIN, HIGH);  // turn on Piezo Buzzer
      }
                // Print the BPM value
      
    }

  } else if (touchState == LOW) {
    Serial.println("The sensor is untouched");
    digitalWrite(BUZZER_PIN, LOW);  // turn off Piezo Buzzer
  }
}
