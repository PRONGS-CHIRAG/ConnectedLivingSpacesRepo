#define LED 2
#define SENSOR 4
#define DELAY 500
//Link : https://github.com/pcbreflux/espressif/blob/master/esp32/arduino/sketchbook/ESP32_MQ_Sensor/ESP32_MQ_Sensor.ino
int sensorValue = 0;  // variable to store the value coming from the sensor

void setup() {
  pinMode(LED, OUTPUT);
  Serial.begin(115200);
  Serial.println("Sensor start");
}

void loop() {
  digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(DELAY);              // wait for a second
  
  // read the value from the sensor:
  sensorValue = analogRead(SENSOR);
  Serial.print("Value: "); Serial.println(sensorValue);
 
  digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
  delay(DELAY);
}
