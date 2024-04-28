#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char *ssid = "WiFi-Led";
const char *password = "123456789";
int LedPin = D2; // Change this to the pin where your LED is connected

ESP8266WebServer server(80);

void handleRoot() {
  String page = "<!DOCTYPE html><html><head><title>LED Control</title>";
  page += "<style>body{font-family: Arial, sans-serif;margin: 0;padding: 20px;text-align: center;}.container{max-width: 400px;margin: 0 auto;}.button{display: inline-block;padding: 10px 20px;font-size: 16px;border: none;cursor: pointer;border-radius: 5px;text-align: center;text-decoration: none;}.button-on{background-color: #4CAF50;color: white;}.button-off{background-color: #f44336;color: white;}</style>";
  page += "</head><body><div class=\"container\"><h1>LED Control</h1>";
  page += "<button class=\"button button-on\" onclick=\"turnOnLED()\">Turn ON</button>";
  page += "<button class=\"button button-off\" onclick=\"turnOffLED()\">Turn OFF</button>";
  page += "<script>function turnOnLED() { var xhr = new XMLHttpRequest(); xhr.open('GET', '/on', true); xhr.send(); }";
  page += "function turnOffLED() { var xhr = new XMLHttpRequest(); xhr.open('GET', '/off', true); xhr.send(); }</script>";
  page += "</div></body></html>";
  server.send(200, "text/html", page);
}

void handleOn() {
  digitalWrite(LedPin, HIGH);
  server.send(200, "text/plain", "LED ON");
}

void handleOff() {
  digitalWrite(LedPin, LOW);
  server.send(200, "text/plain", "LED OFF");
}

void setup() {
  pinMode(LedPin, OUTPUT);
  digitalWrite(LedPin, LOW);
  
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  server.on("/", HTTP_GET, handleRoot);
  server.on("/on", HTTP_GET, handleOn);
  server.on("/off", HTTP_GET, handleOff);

  server.begin();
}

void loop() {
  server.handleClient();
}
