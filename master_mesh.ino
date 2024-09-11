#include "painlessMesh.h"
#include <ESPAsyncTCP.h>
#include <ArduinoJson.h>

#define MESH_PREFIX "Vinay"
#define MESH_PASSWORD "12345678"
#define MESH_PORT 5555

Scheduler userScheduler; // to control your personal task
painlessMesh mesh;

// Map to store the mapping between user-assigned IDs and PainlessMesh node IDs
std::map<uint32_t, uint32_t> userToMeshNodeIdMap;

// Declare variables
const int currentPin = A0;        // Replace with your actual pin numbers
const int voltagePin = A0;
const int ldrPin = D0;
const float voltageReference = 5.0; // Replace with your actual voltage reference
const float sensitivity = 1.0;      // Replace with your actual sensitivity

// Declare sendMessage function prototype
void sendMessage();

Task taskSendMessage(TASK_SECOND *1, TASK_FOREVER, &sendMessage);

void sendMessage()
{
  DynamicJsonDocument jsonDoc(256); // Adjust the size based on your data
  JsonObject jsonData = jsonDoc.to<JsonObject>();

  // Use a user-assigned ID (e.g., 1001) instead of mesh.getNodeId()
  jsonData["Id"] = 1; // Replace with your own logic to assign unique IDs
  jsonData["current"] = analogRead(currentPin);
  jsonData["voltage"] = analogRead(voltagePin);
  jsonData["ldr"] = digitalRead(ldrPin);

  String jsonString;
  serializeJson(jsonData, jsonString);

  mesh.sendBroadcast(jsonString);
  taskSendMessage.setInterval(random(TASK_SECOND * 1, TASK_SECOND * 5));
}

void receivedCallback(uint32_t from, String &msg)
{
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());

  // Extract the user-assigned ID from the received JSON
  DynamicJsonDocument jsonDoc(256);
  deserializeJson(jsonDoc, msg);
  uint32_t userAssignedId = jsonDoc["Id"];

  // Map the user-assigned ID to the PainlessMesh node ID
  userToMeshNodeIdMap[userAssignedId] = from;
}

void newConnectionCallback(uint32_t nodeId)
{
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback()
{
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset)
{
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void setup()
{
  Serial.begin(115200);
  mesh.setDebugMsgTypes(ERROR | STARTUP);
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask(taskSendMessage);
  taskSendMessage.enable();
}

void loop()
{
  mesh.update();
  int sensorValue = analogRead(currentPin);
  float voltage = sensorValue * (voltageReference / 1023.0);
  float current = (voltage - (voltageReference / 2.0)) / sensitivity;
  sendMessage();
  delay(1500);
}
