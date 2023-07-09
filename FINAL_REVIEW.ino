#define USE_ARDUINO_INTERRUPTS true
#include <SoftwareSerial.h>
SoftwareSerial sim800l(10, 11);
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <String.h>
int RXPin = 4;
int TXPin = 3;

const int BUZZER_PIN = 8;  // Arduino pin connected to Buzzer's pin

double longi = 0;
double lati = 0;

int GPSBaud = 9600;

// Create a TinyGPS++ object
TinyGPSPlus gps;

// Create a software serial port called "gpsSerial"
SoftwareSerial gpsSerial(RXPin, TXPin);


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

float latitude, longitude;
String  lat_str , lng_str;


void setup() {
  sim800l.begin(9600);
  gpsSerial.begin(GPSBaud);
  Serial.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT);  // set arduino pin to output mode
  Serial.println("successfully Initialized....");

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
  bool newdata = false;


  currentState = digitalRead(TOUCH_PIN);
  while (gpsSerial.available() > 0) {
    currentState = digitalRead(TOUCH_PIN);

    // If 5000 milliseconds pass and there are no characters coming in
    // over the software serial port, show a "No GPS detected" error


    if (currentState == HIGH && gps.encode(gpsSerial.read())) {

      if (millis() > 5000 && gps.charsProcessed() < 10) {
        Serial.println("No GPS detected");
      }

      Serial.print(" Sensor is touched ");

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
      if (xValue < minVal || xValue > MaxVal || yValue < minVal || yValue > MaxVal || zValue < minVal || zValue > MaxVal && gps.location.isValid()) {  //in case of accident calling to a number 5 times if needed we can add multiple numbers.
        if (sim800l.available())
          Serial.write(sim800l.read());

        latitude = gps.location.lat();
        lat_str = String(latitude, 6);
        longitude = gps.location.lng();
        lng_str = String(longitude, 6);
        Serial.print("Accident detected \n");
        digitalWrite(BUZZER_PIN, HIGH);  // turn on Piezo Buzzer
        delay(5000);
        digitalWrite(BUZZER_PIN, LOW);  // turn on Piezo Buzzer
        Serial.println("Sending SMS...");
        sim800l.print("AT+CMGF=1\r");
        delay(100);
        sim800l.print("AT+CMGS=\"+919511828474\"\r");  //EX +919876543210
        delay(500);
        sim800l.print("ACCIDENT DETECTED ");
        sim800l.print("http://maps.google.com/maps?q=loc:");
        sim800l.print(gps.location.lat(), 6);
        sim800l.print(",");
        sim800l.print(gps.location.lng(), 6);


        sim800l.println("AT");
        delay(1000);

        sim800l.println("AT+CPIN?");
        delay(1000);

        sim800l.println("AT+CREG?");
        delay(1000);

        sim800l.println("AT+CGATT?");
        delay(1000);

        sim800l.println("AT+CIPSHUT");
        delay(1000);

        sim800l.println("AT+CIPSTATUS");
        delay(2000);

        sim800l.println("AT+CIPMUX=0");
        delay(2000);

        ShowSerialData();

        // gprsSerial.println("AT+CSTT=\"airtelgprs.com\"");//start task and setting the APN,
        // delay(1000);

        ShowSerialData();

        sim800l.println("AT+CIICR");  //bring up wireless connection
        delay(3000);

        ShowSerialData();

        sim800l.println("AT+CIFSR");  //get local IP adress
        delay(2000);

        ShowSerialData();

        sim800l.println("AT+CIPSPRT=0");
        delay(3000);

        ShowSerialData();

        sim800l.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"");  //start up the connection
        delay(6000);

        ShowSerialData();

        sim800l.println("AT+CIPSEND");  //begin send data to remote server
        delay(4000);
        ShowSerialData();

        String str = "GET https://api.thingspeak.com/update?api_key=PZK000KI4IM20IIA&field1=" + String(xValue) + "&field2=" + String(yValue) + "&field3=" + String(zValue) + "&field4=" + String(lat_str) + "&field5=" + String(lng_str);
        Serial.println(str);
        sim800l.println(str);  //begin send data to remote server

        delay(4000);
        ShowSerialData();

        sim800l.print((char)26);
        delay(500);
        sim800l.println();
        Serial.println("Text Sent.");
      }
    }
  }
}



void displayInfo() {
  if (gps.location.isValid()) {

    lati = gps.location.lat();
    longi = gps.location.lng();
    Serial.print("Latitude: ");
    Serial.println(lati);
    Serial.println(gps.location.lat(), 6);
    Serial.print("Longitude: ");
    Serial.println(longi);
    Serial.println(gps.location.lng(), 6);
    Serial.print("Altitude: ");
    Serial.println(gps.altitude.meters());


  } else {
    Serial.println("Location: Not Available");
  }
}

void ShowSerialData() {
  while (sim800l.available() != 0)
    Serial.write(sim800l.read());
  delay(5000);
}
