#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char *ssid = "LED_Controller";
const char *password = "password";
const int LedPin = D2;   // Pin connected to the LED
const int LdrPin = A0;   // Pin connected to the LDR sensor
const int threshold = 250; // Threshold value for controlling the LED

ESP8266WebServer server(80);

void handleRoot() {
  String page = "<!DOCTYPE html><html><head><title>LED Control</title></head><body>";
  page += "<h1>LED Control</h1>";
  page += "<p>Light Intensity: <span id=\"ldrValue\">0</span></p>";
  page += "<script>function updateLDR(value) { document.getElementById('ldrValue').textContent = value; }</script>";
  page += "<script>setInterval(function() { var xhr = new XMLHttpRequest(); xhr.onreadystatechange = function() { if (xhr.readyState == XMLHttpRequest.DONE) { document.getElementById('ldrValue').textContent = xhr.responseText; if (xhr.responseText >= " + String(threshold) + ") { document.body.style.backgroundColor = 'yellow'; } else { document.body.style.backgroundColor = 'white'; } } }; xhr.open('GET', '/brightness', true); xhr.send(); }, 1000); </script>";
  page += "</body></html>";

  server.send(200, "text/html", page);
}

void handleBrightness() {
  int ldrValue = analogRead(LdrPin); // Read the value from the LDR sensor
  server.send(200, "text/plain", String(ldrValue)); // Send the LDR value
  if (ldrValue <= threshold) {
    digitalWrite(LedPin, HIGH); // Turn on the LED
  } else {
    digitalWrite(LedPin, LOW); // Turn off the LED
  }
}

void setup() {
  pinMode(LedPin, OUTPUT);
  digitalWrite(LedPin, LOW);
  
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  
  server.on("/", HTTP_GET, handleRoot);
  server.on("/brightness", HTTP_GET, handleBrightness);
  
  server.begin();
}

void loop() {
  server.handleClient();
}
