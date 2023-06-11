#define TINY_GSM_MODEM_SIM800

#include <SoftwareSerial.h>
#include <TinyGsmClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

char output[100];

SoftwareSerial SerialAT(10, 11);  // RX, TX

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

TinyGsm modem(SerialAT);
TinyGsmClient client(modem);
PubSubClient mqtt(client);
StaticJsonDocument<200> doc;

void setup() {
  Serial.begin(9600);
  SerialAT.begin(9600);
  
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
}

void loop() {
  if (Serial.available()) {
    delay(10);
    String message = "RITVIK MC";
    while (Serial.available()) message += (char)Serial.read();
    mqtt.publish(topicOut, message.c_str());
  }

  if (mqtt.connected()) {
    mqtt.loop();
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
  
  doc["LAT"]=0;
  doc["LONG"]=0;
  doc["helmetId"]=23111;
  serializeJson(doc, output);
 
  String message = "RITVIK MC";
  while (Serial.available()) message += (char)Serial.read();
  mqtt.publish(topicOut, output);
  Serial.print("Message sent back: "); 
}
