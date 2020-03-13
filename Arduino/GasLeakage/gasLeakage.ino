int LED = 2;
int SENSOR = 4;
int DELAY =60000;
int sensorValue = 0;
#include <WiFi.h>
#include <PubSubClient.h>
const char* ssid = "PESU-Element Block";
const char* password = "";
const char* mqtt_server = "10.3.32.59";
int p=0;
const char s[]="Alert: Gas leaking";
const char a[]="Regular gas levels";
WiFiClient ESP32Client;
PubSubClient client(ESP32Client);
const int door2 = 2;
long lastMsg = 0;
long now = millis();
long lastMeasure = 0;
int leakage = 0;
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
  pinMode(LED, OUTPUT);
  Serial.begin(115200);
  Serial.println("Sensor start");
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  delay(1000);
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
   delay(5000);              
  sensorValue = analogRead(SENSOR);
  Serial.print("Value: "); 
  Serial.println(sensorValue);  
  delay(1000);
  if(sensorValue>1400){
    digitalWrite(LED,HIGH);
    Serial.println("Gas leak");
    leakage += 1;
    client.publish("GasStatus",s);
  }
  else{
    digitalWrite(LED,LOW);
    leakage += 0;
    client.publish("GasStatus",a);
  }
  char countleakagestring[8];
  dtostrf(leakage,1,2,countleakagestring);
  client.publish("GasLeakageCount",countleakagestring);
  //client.publish("Stat",s);
  delay(2000);
  }
}
