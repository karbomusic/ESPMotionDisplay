#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

ESP8266WebServer server(80);

bool frontyardTriggered = false;
bool kitchenTriggered = false;
bool mediaTriggered = false;

void handleRoot() {
 //digitalWrite(led, 1);
  server.send(200, "text/plain", "I am the motion display server.");
 // digitalWrite(led, 0);
}

void handleNotFound() {
  //digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
 // digitalWrite(led, 0);
}

void initWebServer() {

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/frontyard", []() {
    frontyardTriggered = true;
    server.send(200, "text/plain", "frontyard triggered.");
  });

  server.on("/kitchen", []() {
    kitchenTriggered = true;
    server.send(200, "text/plain", "kitchen triggered.");
  });

  server.on("/media", []() {
    mediaTriggered = true;
    server.send(200, "text/plain", "media triggered.");
  });
  
  server.onNotFound(handleNotFound);
  
  server.begin();
  Serial.println("HTTP server started");
}

void loopWebServer() {
  server.handleClient();
}
