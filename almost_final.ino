#define USE_ARDUINO_INTERRUPTS true
#include <SoftwareSerial.h>
SoftwareSerial sim800l(10, 11);
#include <SoftwareSerial.h>
#include <TinyGPS.h>
SoftwareSerial mySerial(7, 8);
TinyGPS gps;

void gpsdump(TinyGPS &gps);

const int TOUCH_PIN = 6;

#define x A1
#define y A2
#define z A3

int xsample = 0;
int ysample = 0;
int zsample = 0;

#define samples 10
#define minVal -50
#define MaxVal 50

int lastState = LOW;  // the previous state from the input pin
int currentState;     // the current reading from the input pin


void setup()
{
  sim800l.begin(9600);
  mySerial.begin(9600);
  Serial.begin(9600);
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

void loop()
{

  currentState = digitalRead(TOUCH_PIN);
  while (mySerial.available()) {
    currentState = digitalRead(TOUCH_PIN);

    // If 5000 milliseconds pass and there are no characters coming in
    // over the software serial port, show a "No GPS detected" error


    if (currentState == HIGH) {

      Serial.print(" Sensor is touched ");

      bool newdata = false;

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
        loops();

      }
    }
  }

}
void loops()
{
  bool newdata = false;
  unsigned long start = millis();
  // Every 5 seconds we print an update

  char c = mySerial.read();
  Serial.print(c);  // uncomment to see raw GPS data
  if (gps.encode(c))
  {
    Serial.print("check");
    newdata = true;

  }

  if (newdata)
  {
    gpsdump(gps);
    Serial.println();
  }
}
void gpsdump(TinyGPS &gps)
{
  long lat, lon;
  float flat, flon;
  unsigned long age;
  gps.f_get_position(&flat, &flon, &age);
  Serial.println("Sending SMS...");
  sim800l.print("AT+CMGF=1\r");
  delay(100);
  sim800l.print("AT+CMGS=\"+919511828474\"\r");//EX +919876543210
  delay(500);
  sim800l.print("ACCIDENT DETECTED ");
  sim800l.print("http://maps.google.com/maps?q=loc:");
  sim800l.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
  sim800l.print(",");
  sim800l.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
  sim800l.print((char)26);
  delay(500);
  sim800l.println();
  Serial.println("Text Sent.");
  delay(5000);
}



