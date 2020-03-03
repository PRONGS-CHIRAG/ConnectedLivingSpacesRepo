#include <WiFi.h>
#include <PubSubClient.h>
#include"TimeLib.h"
#include "NTPClient.h"
#include <WiFiUdp.h>
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
String formattedDate,dayStamp,timeStamp;
const int offset=19800;
const char* ssid = "PESU-Element Block";
const char* password = "";
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;
int buzzer=23;
const char* mqtt_server = "10.3.32.59";
WiFiClient iotmain;
PubSubClient client(iotmain);
const int door1 = 4;
const int door2 = 15;
const int door3 = 2;
int p=0;
int room1_unlock_count=0,room1_lock_count=0,room2_unlock_count=0,room2_lock_count=0;
String  in_timestamp,out_timestamp,in_timestamp2,out_timestamp2;
long no = millis();
long lastMeasure = 0;
void op_buzz(){
  digitalWrite(door2,LOW);
  delay(700);
  digitalWrite(door2,HIGH);
  }
void cl_buzz(){
   digitalWrite(door2,LOW);
  delay(700);
  digitalWrite(door2,HIGH);
  delay(700);
  digitalWrite(door2,LOW);
  delay(700);
  digitalWrite(door2,HIGH);
 
}
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
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
  timeClient.begin();
  timeClient.setTimeOffset(offset);
 }
void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  for(int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  if(topic=="iot/maindoor"){
      Serial.print("Opening iot main door ");
      if(messageTemp == "true"){
        digitalWrite(door1, HIGH);
        room1_lock_count += 1;
        formattedDate = timeClient.getFormattedDate();
        int splitT = formattedDate.indexOf("T");
        in_timestamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
        Serial.println("Timestamp");
        Serial.println(in_timestamp);
        op_buzz();
}
      else if(messageTemp == "false"){
        digitalWrite(door1, LOW);
        Serial.print("Closing iot main door");
        room1_unlock_count += 1;
        formattedDate = timeClient.getFormattedDate();
        int splitT = formattedDate.indexOf("T");
        out_timestamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
        Serial.println("Timestamp");
        Serial.println(out_timestamp);
        cl_buzz();
       }
}
 
 /*if(topic=="iot/cabin"){
      Serial.print("Opening iot main door b ");
      if(messageTemp == "true"){
        digitalWrite(door2, HIGH);
        room2_lock_count += 1;
        timeClient.begin();
         timeClient.setTimeOffset(offset);
        formattedDate = timeClient.getFormattedDate();
        int splitT = formattedDate.indexOf("T");
        in_timestamp2 = formattedDate.substring(splitT+1, formattedDate.length()-1);
        Serial.println("Timestamp");
        Serial.println(in_timestamp2);
         
        //op_buzz();
       
      }
      else if(messageTemp == "false"){
        digitalWrite(door2, LOW);
        Serial.print("Off");
        room2_unlock_count += 1;
        timeClient.begin();
         timeClient.setTimeOffset(offset);
        formattedDate = timeClient.getFormattedDate();
        int splitT = formattedDate.indexOf("T");
        out_timestamp2 = formattedDate.substring(splitT+1, formattedDate.length()-1);
        Serial.println("Timestamp");
        Serial.println(out_timestamp2);
        //cl_buzz();
       }
  }*/
char lock1string[8],unlock1string[8],intime1[16],outtime1[16],lock2string[8],unlock2string[8],intime2[16],outtime2[16];
dtostrf(room1_lock_count, 1, 2,lock1string );
dtostrf(room1_unlock_count, 1, 2, unlock1string);
/*dtostrf(room2_lock_count, 1, 2,lock2string );
dtostrf(room2_unlock_count, 1, 2, unlock2string);*/
in_timestamp.toCharArray(intime1,16);
out_timestamp.toCharArray(outtime1,16);
/*in_timestamp2.toCharArray(intime2,16);
out_timestamp2.toCharArray(outtime2,16);*/ 
client.publish("Lock/iotmain",lock1string);
client.publish("Unlock/iotmain",unlock1string);
//client.publish("Lock/cabin",lock2string);
//client.publish("Unlock/cabin",unlock2string);
client.publish("iotmain/in",intime1);
client.publish("iotmain/out",outtime1);
//client.publish("cabin/in",intime2);
//client.publish("cabin/out",outtime2);
}
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    digitalWrite(door3,HIGH);
    delay(2000);
    digitalWrite(door3,LOW);
    if (client.connect("iotmain")) {
      Serial.println("connected");  
      digitalWrite(door3,HIGH);
      client.subscribe("iot/maindoor");
      //client.subscribe("iot/cabin");
      } 
    else {
      Serial.print("failed, rc=");
      ESP.restart();
      Serial.print(client.state());
      Serial.println(" try again in 4 seconds");
      delay(4000);
      
    }
  }
}
void setup() {
  pinMode(door1, OUTPUT);
  pinMode(door2, OUTPUT);
  pinMode(door3, OUTPUT);
  digitalWrite(door1,HIGH);
  Serial.println("Lock on");
  //digitalWrite(door2,HIGH);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}
void loop() {
if (!client.connected()) {
    reconnect();
}
  if(!client.loop())
    client.connect("ESP32Client");
} 
