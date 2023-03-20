#include <math.h>
#define USE_ARDUINO_INTERRUPTS true    // Set-up low-level interrupts for most acurate BPM maths.
#include <PulseSensorPlayground.h>     // Includes the PulseSensorPlayground Library. 
const int SENSOR_PIN = 7; // the Arduino's input pin that connects to the sensor's SIGNAL pin
const int x_out = A1; /* connect x_out of module to A1 of UNO board */
const int y_out = A2; /* connect y_out of module to A2 of UNO board */
const int z_out = A3; /* connect z_out of module to A3 of UNO board */
const int PulseWire = 0;       // PulseSensor PURPLE WIRE connected to ANALOG PIN 0
const int LED13 = 13;          // The on-board Arduino LED, close to PIN 13.
int Threshold = 550;           // Determine which Signal to "count as a beat" and which to ignore.

PulseSensorPlayground pulseSensor;


// Variables will change:
int lastState = LOW;      // the previous state from the input pin
int currentState;         // the current reading from the input pin

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  // initialize the Arduino's pin as an input
  pinMode(SENSOR_PIN, INPUT);
  pulseSensor.analogInput(PulseWire);
  pulseSensor.blinkOnPulse(LED13);       //auto-magically blink Arduino's LED with heartbeat.
  pulseSensor.setThreshold(Threshold);
  // Double-check the "pulseSensor" object was created and "began" seeing a signal.
  if (pulseSensor.begin()) {
    Serial.println("We created a pulseSensor Object !");  //This prints one time at Arduino power-up,  or on Arduino reset.
  }
}

void loop() {
  // read the state of the the input pin:
  currentState = digitalRead(SENSOR_PIN);

  int myBPM = pulseSensor.getBeatsPerMinute();  // Calls function on our pulseSensor object that returns BPM as an "int".
  // "myBPM" hold this BPM value now.
  if (pulseSensor.sawStartOfBeat()) {            // Constantly test to see if "a beat happened".
    Serial.println("A HeartBeat Happened ! "); // If test is "true", print a message "a heartbeat happened".
    Serial.print("BPM: ");                        // Print phrase "BPM: "
    Serial.println(myBPM);                        // Print the value inside of myBPM.
  }

  if (lastState == LOW && currentState == HIGH)
  {
    Serial.println("The sensor is touched");
    Serial.print("\n");
    int x_adc_value, y_adc_value, z_adc_value;
 
    x_adc_value = analogRead(x_out);
    y_adc_value = analogRead(y_out);
    z_adc_value = analogRead(z_out);
    Serial.print("x = ");
    Serial.print(x_adc_value);
    Serial.print("\t\t");
    Serial.print("y = ");
    Serial.print(y_adc_value);
    Serial.print("\t\t");
    Serial.print("z = ");
    Serial.print(z_adc_value);
    Serial.print("\t\t");

    Serial.print("\n");




  }

  lastState = currentState;


  delay(1000);
}
