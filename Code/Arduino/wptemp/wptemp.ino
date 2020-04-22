#include <WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
const char* ssid = "PESU-Element Block";
const char* password = "";
const char* mqtt_server = "10.3.32.59";
const int fan = 18;
const int pwm = 9;
const int heater = 14;
int p=0;
WiFiClient TEMPClient;
PubSubClient client(TEMPClient);
const int door2 = 2;
long lastMsg = 0;
long now = millis();
long lastMeasure = 0;
#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
int numberOfDevices;
DeviceAddress tempDeviceAddress;
unsigned char devices[10];
float temperaturesC[10], temperaturesF[10], zonea, zoneb, zonec, zoned;
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
     p += 1;
     if(p>10){
      ESP.restart();
     }
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}
void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
}
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      digitalWrite(door2, HIGH);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      ESP.restart();
      delay(5000);
    }
  }
}
void setup() {
  pinMode(fan, OUTPUT);
  pinMode(door2, OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  Serial.begin(9600);
  sensors.begin();
  numberOfDevices = sensors.getDeviceCount();
  Serial.print("Locating devices...");
  Serial.print("Found ");
  Serial.print(numberOfDevices, DEC);
  Serial.println(" devices.");
  for (int i = 0; i < numberOfDevices; i++) {
    if (sensors.getAddress(tempDeviceAddress, i)) {
      Serial.print("Found device ");
      Serial.print(i, DEC);
      Serial.print(" with address: ");
      devices[i] = printAddress(tempDeviceAddress);
      Serial.println();
    } else {
      Serial.print("Found ghost device at ");
      Serial.print(i, DEC);
      Serial.print(" but could not detect address. Check power and cabling");
    }
  }
}
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  if (!client.loop())
    client.connect("ESP32Client");
  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    sensors.requestTemperatures();
    for (int i = 0; i < numberOfDevices; i++) {
      if (sensors.getAddress(tempDeviceAddress, i)) {
        Serial.print("Temperature for device: ");
        Serial.println(i, DEC);
        float tempC = sensors.getTempC(tempDeviceAddress);
        temperaturesC[i] = tempC;
        Serial.print("Temp C: ");
        Serial.print(tempC);
        Serial.print(" Temp F: ");
        Serial.println(DallasTemperature::toFahrenheit(tempC));
        temperaturesF[i] = DallasTemperature::toFahrenheit(tempC); // Converts tempC to Fahrenheit
      }
    }
    float meantempC = 0.0, meantempF = 0.0;
    for ( int i = 0; i < numberOfDevices; i++) {
      meantempC += temperaturesC[i];
      meantempF += temperaturesF[i];
    }
    zonea = temperaturesC[0];
    zoneb = temperaturesC[1];
    zonec = temperaturesC[2];
    zoned = temperaturesC[3];
    meantempC = meantempC / numberOfDevices;
    meantempF = meantempF / numberOfDevices;
    Serial.println("Mean temperature in Celsius");
    Serial.print(meantempC);
    Serial.println();
    Serial.println("Mean temperature in Fahrenheit");
    Serial.println(meantempF);
    if (meantempC >= 28) {
      Serial.println("Turning fan on");
      digitalWrite(fan, HIGH);
    }
    else {
      Serial.println("Turning fan off");
      digitalWrite(fan, LOW);
    }
    if(meantempC<=18){
      Serial.println("Turning heater on");
      digitalWrite(heater,HIGH);
    }
    else{
      Serial.println("Turning heater off");
      digitalWrite(heater,LOW);
    }
    if(meantempC>=28 && meantempC <31){
      analogWrite(pwm,51);
    }
    else if(meantempC>=31 && meantempC <34){
      analogWrite(pwm,102);
    }
    else if(meantempC>=34 && meantempC <37){
      analogWrite(pwm,153);
    }
    else if(meantempC>=37 && meantempC <40){
      analogWrite(pwm,153);
    }
    else if(meantempC>=40 && meantempC<43){
      analogWrite(pwm,204);
    }
    char tempCString[8], tempFString[8], zoneas[8], zonebs[8], zonecs[8], zoneds[8];
    dtostrf(meantempC, 1, 2, tempCString);
    dtostrf(meantempF, 1, 2, tempFString);
    dtostrf(zonea, 1, 2, zoneas);
    dtostrf(zoneb, 1, 2, zonebs);
    dtostrf(zonec, 1, 2, zonecs);
    dtostrf(zoned, 1, 2, zoneds);
    client.publish("CelsiusReading", tempCString);
    client.publish("FahrenheitReading", tempFString);
    client.publish("ZoneAReading", zoneas);
    client.publish("ZoneBReading", zonebs);
    client.publish("ZoneCReading", zonecs);
    client.publish("ZoneDReading", zoneds);
    delay(5000);
  }
}
unsigned char printAddress(DeviceAddress deviceAddress) {
  for (uint8_t i = 0; i < 8; i++) {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
    return (deviceAddress[i]);
  }
}
