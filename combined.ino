#define USE_ARDUINO_INTERRUPTS true
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

const int TOUCH_PIN = 7;
#include <PulseSensorPlayground.h>

#define x A1
#define y A2
#define z A3

int xsample = 0;
int ysample = 0;
int zsample = 0;

#define samples 10
#define minVal -50
#define MaxVal 50

// Choose two Arduino pins to use for software serial
int RXPin = 4;
int TXPin = 3;

int GPSBaud = 9600;

// Create a TinyGPS++ object
TinyGPSPlus gps;

// Create a software serial port called "gpsSerial"
SoftwareSerial gpsSerial(RXPin, TXPin);

int lastState = LOW;  // the previous state from the input pin
int currentState;     // the current reading from the input pin

void setup() {
  // Start the Arduino hardware serial port at 9600 baud
  Serial.begin(9600);
  // Start the software serial port at the GPS's default baud
  gpsSerial.begin(GPSBaud);

  Serial.println("successfully Initialized....");
  pinMode(TOUCH_PIN, INPUT);

  for (int i = 0; i < samples; i++) {
    xsample += analogRead(x);
    ysample += analogRead(y);
    zsample += analogRead(z);
  }

  xsample /= samples;
  ysample /= samples;
  zsample /= samples;

  Serial.println(xsample);
  Serial.println(ysample);
  Serial.println(zsample);
  delay(1000);
}

void loop() {

  currentState = digitalRead(TOUCH_PIN);
  while (gpsSerial.available() > 0) {
    currentState = digitalRead(TOUCH_PIN);



    // If 5000 milliseconds pass and there are no characters coming in
    // over the software serial port, show a "No GPS detected" error


    if (currentState == HIGH && gps.encode(gpsSerial.read())) {
      
      Serial.print(" Sensor is touched ");

      if (millis() > 5000 && gps.charsProcessed() < 10) {
        Serial.println("No GPS detected");
      }
      int value1 = analogRead(x);
      int value2 = analogRead(y);
      int value3 = analogRead(z);

      int xValue = xsample - value1;
      int yValue = ysample - value2;
      int zValue = zsample - value3;

      Serial.print("x=");
      Serial.println(xValue);
      Serial.print("y=");
      Serial.println(yValue);
      Serial.print("z=");
      Serial.println(zValue);
      delay(2000);
      //condition for accident detection
      if (xValue < minVal || xValue > MaxVal || yValue < minVal || yValue > MaxVal || zValue < minVal || zValue > MaxVal) {  //in case of accident calling to a number 5 times if needed we can add multiple numbers.
        Serial.print("Accident detected \n");
        displayInfo();
        delay(5000);
      }
    }
  }
}

void displayInfo() {
  if (gps.location.isValid()) {
    Serial.print("Latitude: ");
    Serial.println(gps.location.lat(), 6);
    Serial.print("Longitude: ");
    Serial.println(gps.location.lng(), 6);
    Serial.print("Altitude: ");
    Serial.println(gps.altitude.meters());
  } else {
    Serial.println("Location: Not Available");
  }
}
