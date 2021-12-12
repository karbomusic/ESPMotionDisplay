// Group 4 = Media room
// Group 2 = Studio

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include "WebServer.h"             // in .ino directory

#define s Serial

ESP8266WiFiMulti WiFiMulti;

const char* ssid     = "IOTNET2G";
const char* password = "creekvalley124";
const char* host = "192.168.2.101";
const String url = "http://192.168.2.56/api/IgniteUser/groups/4/action";
const String softwareVersion = "04-10-2018";                                // last update
const String hostName = "ESPMotion-Display";                                // name as seen on network
const String deviceFamily  = "MOTIONDISPLAY";
String jsonToken = "{""\"on\":<value>}";
String json;

const int redPin = 14;
const int bluePin = 12;
const int greenPin = 13;
int lowHigh;

const long noMotionDuration = 60000 * 20;                                    // How long without motion before turning off -> minute * value = minutes
unsigned long lastMotionEvent = millis();

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
    delay(1000);
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
    delay(1000);
  }

  Serial.println("Waiting for motion...");
}

void loop() {

  loopWebServer();

 if(frontyardTriggered)
 {
  Serial.println("Front yard motion triggered!");
  digitalWrite(greenPin, HIGH);
  delay(1000);
  digitalWrite(greenPin, LOW);
  frontyardTriggered = false;
 }

 if(mediaTriggered)
 {
  Serial.println("Media room motion triggered!");
  digitalWrite(bluePin, HIGH);
  delay(1000);
  digitalWrite(bluePin, LOW);
  mediaTriggered = false;
 }

  if(kitchenTriggered)
 {
  Serial.println("Kitchen motion triggered!");
  digitalWrite(redPin, HIGH);
  delay(1000);
  digitalWrite(redPin, LOW);
  kitchenTriggered = false;
 }
}

void checkForUpdate()
{
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
  lastMotionEvent = millis();
}

