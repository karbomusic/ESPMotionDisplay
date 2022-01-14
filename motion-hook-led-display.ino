

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include "WebServer.h"             // in .ino directory

#define s Serial

ESP8266WiFiMulti WiFiMulti;

const char* ssid     = "YOURSSID";
const char* password = "YOURPASSWORD";
const char* host = "192.168.2.101";

const String softwareVersion = "12-12-2021";                                // last update
const String hostName = "ESPMotion-Display";                                // name as seen on network
const String deviceFamily  = "MOTIONDISPLAY";

const int redPin = 14;
const int bluePin = 12;
const int greenPin = 13;

void setup() {

  pinMode(redPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(greenPin, OUTPUT);

  digitalWrite(redPin, LOW);
  digitalWrite(bluePin, LOW);
  digitalWrite(greenPin, LOW);
  
  WiFi.enableAP(false);
  WiFi.persistent(true);
  WiFi.enableSTA(true);

  s.begin(115200);
  s.println("\r\n\r\n");

  for (uint8_t t = 4; t > 0; t--) {
    s.printf("[SETUP] WAIT %d...\n", t);
    s.flush();
    digitalWrite(bluePin, HIGH);
    delay(250);
    digitalWrite(bluePin, LOW);
    delay(250);
  }

  Serial.println("\nConnecting to WiFi...\n");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  WiFi.hostname(hostName);
  Serial.println("\n-------------------------------------");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Hostname: ");
  Serial.println(WiFi.hostname());
  Serial.println("Device Family: " + deviceFamily);
  Serial.println("-------------------------------------\n");

  checkForUpdate();
  initWebServer();

  Serial.println("Run away, arming in 5 seconds...");

  for (int i = 5; i > 0; i--)
  {
    Serial.print((String)i + " ");
    digitalWrite(greenPin, HIGH);
    delay(500);
    digitalWrite(greenPin, LOW);
    delay(500);
  }

  Serial.println("Waiting for motion...");
}

void loop() {

  loopWebServer();

 if(frontyardTriggered)
 {
  Serial.println("Front yard motion triggered!");
  for(int i=0;i<10;i++)
  {
    digitalWrite(greenPin, HIGH);
    delay(20);
    digitalWrite(greenPin, LOW);
    delay(20);
  }
  frontyardTriggered = false;
 }

 if(mediaTriggered)
 {
  Serial.println("Media room motion triggered!");
  for(int i=0;i<10;i++)
  {
    digitalWrite(bluePin, HIGH);
    delay(20);
    digitalWrite(bluePin, LOW);
    delay(20);
  }
  mediaTriggered = false;
 }

  if(kitchenTriggered)
 {
  Serial.println("Kitchen motion triggered!");
  for(int i=0;i<10;i++)
  {
    digitalWrite(redPin, HIGH);
    delay(20);
    digitalWrite(redPin, LOW);
    delay(20);
  }
  kitchenTriggered = false;
 }

 if(greatRoomTriggered)
 {
  Serial.println("Great room motion triggered!");
  for(int i=0;i<10;i++)
  {
    digitalWrite(redPin, HIGH);
    digitalWrite(greenPin, HIGH);
    delay(20);
    digitalWrite(redPin, LOW);
    digitalWrite(greenPin, LOW);
    delay(20);
  }

  greatRoomTriggered = false;
 }
}

void checkForUpdate()
{
  //not used yet for this project
  return;
  Serial.println("Current Version: [" + softwareVersion + "] \nChecking for update...\n");
  t_httpUpdate_return ret = ESPhttpUpdate.update("192.168.2.101", 80, "/themothership/api/admin/UpdateESP8266OTA?softwareVersion=" + softwareVersion + "&deviceFamily=" + deviceFamily);

  switch (ret) {
    case HTTP_UPDATE_FAILED:
      Serial.println("Update failed.");
      Serial.printf("Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
      break;
    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("No update needed.");
      break;
    case HTTP_UPDATE_OK:
      Serial.println("Update ok."); // may not called we reboot the ESP
      break;
  }

}

