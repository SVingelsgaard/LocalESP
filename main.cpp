#include <Arduino.h>
#include <WiFi.h>
#include "wifi_credentials.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

int doorPin = 13;  // The D13 pin

AsyncWebServer server(80);//server instance

String requestBody = ""; // to store the incoming request body

void handlePostRequest(AsyncWebServerRequest *request) {
  if (request->method() == HTTP_POST) {
    if (requestBody.length() > 0) {
      Serial.println(requestBody);

      // Use ArduinoJson to deserialize the payload
      StaticJsonDocument<256> doc; 
      DeserializationError error = deserializeJson(doc, requestBody);

      if (error) {
        Serial.println("Failed to parse JSON");
        request->send(400, "text/plain", "Bad Request: Invalid JSON format");
        return;
      }

      if (doc.containsKey("opendoor") && doc["opendoor"].as<bool>() == true) {
        
        digitalWrite(doorPin, HIGH);
        delay(500);  // wait for .5 second
        digitalWrite(doorPin, LOW);

        request->send(200, "text/plain", "Door is opened");
      } else {
        Serial.println("opendoor key not found or set to false");
        request->send(400, "text/plain", "Bad Request: 'opendoor' key not found or set to false");
      }

      requestBody = "";  // Clear the body content after processing

    } else {
      Serial.println("No body in the request");
      request->send(400, "text/plain", "Bad Request: No body in the request");
    }
  } else {
    request->send(405, "text/plain", "Method Not Allowed");
  }
}


void setup() {
  Serial.begin(9600);

  //wifi things
  WiFi.begin(ssid, password);//defined in wifi_credential.h file
  Serial.println("Conecting to WiFi...");
  
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConnected to the WiFi");

  //server config
  server.on("/openDoor", HTTP_POST, handlePostRequest)
    .onBody([](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      // Append the incoming data to the requestBody
      for(size_t i=0; i<len; i++) {
        requestBody += (char)data[i];
      }
    });
  server.begin();
  

  //HW config
  pinMode(doorPin, OUTPUT);
  
}

void loop() {
  if (WiFi.status() == WL_CONNECTED){

  }else{
    Serial.print("Connection lost");
  }
}

