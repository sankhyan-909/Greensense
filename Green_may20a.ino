#include "arduino_secrets.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "arduino_secrets.h"
#include "thingProperties.h"
#include <DHT.h>

#define DHTPIN 4       // D4 on NodeMCU ESP8266
#define DHTTYPE DHT11  // DHT11 sensor
#define SOIL_MOISTURE_PIN A0   // A0 on NodeMCU ESP8266
#define RELAY_PIN 5            // D1 on NodeMCU ESP8266

DHT dht(DHTPIN, DHTTYPE);
unsigned long previousMillis = 0;
const long interval = 2000;  // Sensor read interval (2 seconds)

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(SOIL_MOISTURE_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
}

void loop() {
  ArduinoCloud.update();
  
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    // Read humidity and temperature with retry mechanism
    float h = readDHTHumidity();
    float t = readDHTTemperature();
    
    if (!isnan(h) && !isnan(t)) {
      Serial.print("Humidity: "); Serial.print(h); Serial.print("% | ");
      Serial.print("Temperature: "); Serial.print(t); Serial.println("°C");
      humidity = h; temperature = t;
    } else {
      Serial.println("DHT11 sensor error");
    }

    // Read soil moisture
    int soilValue = analogRead(SOIL_MOISTURE_PIN);
    int moisturePercentage = constrain(map(soilValue, 950, 500, 0, 100), 0, 100);
    Serial.print("Soil Moisture: "); Serial.print(moisturePercentage); Serial.println("%");
    soilMoisture = moisturePercentage;
  }
}

// Retry mechanism for DHT sensor readings
float readDHTHumidity() {
  float h;
  for (int i = 0; i < 3; i++) {  
    h = dht.readHumidity();
    if (!isnan(h)) return h;
    delay(500);
  }
  return NAN;  
}

float readDHTTemperature() {
  float t;
  for (int i = 0; i < 3; i++) {  
    t = dht.readTemperature();
    if (!isnan(t)) return t;
    delay(500);
  }
  return NAN;  
}

// Cloud variable callback functions
void onHumidityChange() {}
void onSoilMoistureChange() {}
void onTemperatureChange() {}

void onWaterPumpChange() {
  digitalWrite(RELAY_PIN, waterPump ? LOW : HIGH);
  Serial.println(waterPump ? "Water pump ON" : "Water pump OFF");
}