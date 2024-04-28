#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char *ssid = "WiFi-Led";
const char *password = "123456789";
int LedPin = D2; // Change this to the pin where your LED is connected

ESP8266WebServer server(80);

void setup() {
  pinMode(LedPin, OUTPUT);
  
  // Connect to Wi-Fi
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  // Define the web server routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/on", HTTP_GET, handleOn);
  server.on("/off", HTTP_GET, handleOff);

  // Start the server
  server.begin();
}

void loop() {
  server.handleClient();
}

void handleRoot() {
server.send(200, "text/html", "<h1>Control Panel</h1>");
}

void handleOn() {
  digitalWrite(LedPin, HIGH);
  server.send(200, "text/html", "LED is ON");
}

void handleOff() {
  digitalWrite(LedPin, LOW);
  server.send(200, "text/html", "LED is OFF");
}
