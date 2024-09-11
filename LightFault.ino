#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#define RefVal 5.0

const char* ssid = "Vinay";
const char* password = "12345678";
const char* serverUrl = "http://192.168.228.208:3000/data";

const int currentSensorPin = A0; // Analog pin for current sensor
const int externalSensorPin = 16; // Digital pin for external sensor

const float MAX_CURRENT = 20.0; // Amps

const int averageValue = 500;
long int sensorValue = 0;
float sensitivity = 1000.0 / 200.0;
float Vref = 1508;

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
}

void loop() {
  for (int i = 0; i < averageValue; i++) {
    sensorValue += analogRead(currentSensorPin);
    delay(2);
  }

  sensorValue = sensorValue / averageValue;

  float unitValue= RefVal / 1024.0*1000 ;
  float voltage = unitValue * sensorValue; 

  float current = (voltage - Vref) * sensitivity;

  sensorValue = 0;

  int ldr = digitalRead(externalSensorPin); // Read value from external sensor

  // Create a JSON object
  StaticJsonDocument<200> jsonDoc;
  // Add data to the JSON object
  jsonDoc["Id"] = "3";
  jsonDoc["current"] = current;
  jsonDoc["voltage"] = voltage;
  jsonDoc["ldr"] = ldr; // Add external sensor value to JSON
  jsonDoc["faultType"] = "No Fault";

  // Serialize the JSON object to a string
  String jsonStr;
  serializeJson(jsonDoc, jsonStr);

  // Create a WiFiClient object
  WiFiClient client;

  HTTPClient http;
  http.begin(client, serverUrl);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(jsonStr);

  if (httpResponseCode > 0) {
    Serial.println("HTTP POST Success");
    Serial.println("HTTP Response code: " + String(httpResponseCode));
    Serial.println(http.getString());
  } else {
    Serial.println("HTTP POST Error");
    Serial.println("HTTP Response code: " + String(httpResponseCode));
  }

  http.end();
  delay(2000);
}
