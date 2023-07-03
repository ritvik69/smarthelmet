
#define TINY_GSM_MODEM_SIM800

#include <SoftwareSerial.h>
#include <TinyGsmClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

char output[100];

SoftwareSerial sim800l(10, 11);

//Network details
const char apn[] = "my3g";
const char user[] = "";
const char pass[] = "";

// MQTT details
const char* broker = "i87100c4.emqx.cloud";
const char* topicOut = "shahrulnizam/out";
const char* topicIn = "shahrulnizam/in";
const char* mqttuser = "test";
const char* mqttpass = "test";

TinyGsm modem(sim800l);
TinyGsmClient client(modem);
PubSubClient mqtt(client);
StaticJsonDocument<200> doc;


const int BUZZER_PIN = 8;  // Arduino pin connected to Buzzer's pin

double longi = 0;
double lati = 0;


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


void setup() {
  Serial.begin(9600);
  sim800l.begin(9600);

  pinMode(BUZZER_PIN, OUTPUT);  // set arduino pin to output mode
  Serial.println("successfully Initialized....");
  Serial.println("System start.");
  modem.restart();
  Serial.println("Modem: " + modem.getModemInfo());
  Serial.println("Searching for telco provider.");
  if (!modem.waitForNetwork()) {
    Serial.println("fail");
    while (true)
      ;
  }
  Serial.println("Connected to telco.");
  Serial.println("Signal Quality: " + String(modem.getSignalQuality()));

  Serial.println("Connecting to GPRS network.");
  if (!modem.gprsConnect(apn, user, pass)) {
    Serial.println("fail");
    while (true)
      ;
  }
  Serial.println("Connected to GPRS: " + String(apn));

  mqtt.setServer(broker, 1883);
  mqtt.setCallback(mqttCallback);
  Serial.println("Connecting to MQTT Broker: " + String(broker));
  while (mqttConnect() == false) continue;
  Serial.println();

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

  if (Serial.available()) {
    delay(10);
    String message = "RITVIK MC";
    while (Serial.available()) message += (char)Serial.read();
    mqtt.publish(topicOut, message.c_str());
  }

  if (mqtt.connected()) {
    mqtt.loop();
  }


  // If 5000 milliseconds pass and there are no characters coming in
  // over the software serial port, show a "No GPS detected" error


  int value1 = analogRead(x);
  int value2 = analogRead(y);
  int value3 = analogRead(z);

  int xValue = xsample - value1;
  int yValue = ysample - value2;
  int zValue = zsample - value3;

  // Serial.print("x=");
  // Serial.println(xValue);
  // Serial.print("y=");
  // Serial.println(yValue);
  // Serial.print("z=");
  // Serial.println(zValue);
  // delay(2000);
  doc["x"] = xValue;
  doc["y"] = yValue;
  doc["z"] = zValue;

  if (xValue < minVal || xValue > MaxVal || yValue < minVal || yValue > MaxVal || zValue < minVal || zValue > MaxVal) {  //in case of accident calling to a number 5 times if needed we can add multiple numbers.
    doc["x"] = xValue;
    doc["y"] = yValue;
    doc["z"] = zValue;

    Serial.print("Accident detected \n");

    digitalWrite(BUZZER_PIN, HIGH);  // turn on Piezo Buzzer
    delay(5000);
  } else {
    digitalWrite(BUZZER_PIN, LOW);  // turn on Piezo Buzzer
  }
}

boolean mqttConnect() {
  if (!mqtt.connect("GsmClientTest", mqttuser, mqttpass)) {
    Serial.print(".");
    return false;
  }
  Serial.println("Connected to broker.");
  mqtt.subscribe(topicIn);
  return mqtt.connected();
}

void mqttCallback(char* topic, byte* payload, unsigned int len) {
  Serial.print("Message receive: ");
  Serial.write(payload, len);
  Serial.println();

  doc["LAT"] = lati;
  doc["LONG"] = longi;
  doc["helmetId"] = 23111;
  serializeJson(doc, output);

  String message = "RITVIK MC";
  while (Serial.available()) message += (char)Serial.read();
  mqtt.publish(topicOut, output);
  Serial.print("Message sent back: ");
}

// void displayInfo() {
//   if (gps.location.isValid()) {

//     lati = gps.location.lat();
//     longi = gps.location.lng();
//     Serial.print("Latitude: ");
//     Serial.println(lati);
//     Serial.println(gps.location.lat(), 6);
//     Serial.print("Longitude: ");
//     Serial.println(longi);
//     Serial.println(gps.location.lng(), 6);
//     Serial.print("Altitude: ");
//     Serial.println(gps.altitude.meters());


//   } else {
//     Serial.println("Location: Not Available");
//   }
// }
