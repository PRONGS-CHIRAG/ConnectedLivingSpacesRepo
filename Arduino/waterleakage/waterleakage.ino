// Sensor pins
const int watPin = 4;

// Value for storing water level
int watValue = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
}

void loop() {
  // Reading potentiometer value
  Serial.print("Water level: ");
  watValue = analogRead(watPin);
  Serial.println(watValue);
  delay(500);
}
