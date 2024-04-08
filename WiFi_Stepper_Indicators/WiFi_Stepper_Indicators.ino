#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#ifndef APSSID
#define APSSID "BionicControl"
#define APPSK "123456789"
#endif

const char *ssid = APSSID;
const char *password = APPSK;

const int dirPinMotor1 = D2;
const int stepPinMotor1 = D1;
const int irSensorPinMotor1 = D5;

const int dirPinMotor2 = D7;
const int stepPinMotor2 = D6;
const int irSensorPinMotor2 = D3;

const int stepsPerRevolution = 200;
const int maxRounds = 18; // Maximum number of rounds before moving back

int currentPositionMotor1 = 0;
int currentPositionMotor2 = 0;

ESP8266WebServer server(80);

void setup()
{
  Serial.begin(115200);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.println();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  
  pinMode(irSensorPinMotor1, INPUT_PULLUP);
  pinMode(irSensorPinMotor2, INPUT_PULLUP);

  pinMode(dirPinMotor1, OUTPUT);
  pinMode(stepPinMotor1, OUTPUT);
  pinMode(dirPinMotor2, OUTPUT);
  pinMode(stepPinMotor2, OUTPUT);

  // Initialize motor positions based on IR sensor values
  Serial.println("Motor 1 : ");
  delay(5000);
  calibrateMotorPosition(dirPinMotor1, stepPinMotor1, irSensorPinMotor1);
  currentPositionMotor1=0;
  Serial.println("Motor 2 : ");
  delay(5000);
  calibrateMotorPosition(dirPinMotor2, stepPinMotor2, irSensorPinMotor2); 
  currentPositionMotor2=0;

  server.on("/", HTTP_GET, handleRoot);
  server.on("/FWD1", HTTP_GET, startMotor1);
  server.on("/FWD2", HTTP_GET, startMotor2);
  server.on("/BWD1", HTTP_GET, backMotor1);
  server.on("/BWD2", HTTP_GET, backMotor2);
  server.on("/RST1", HTTP_GET, rstMotor1);
  server.on("/RST2", HTTP_GET, rstMotor2);
  server.begin();
}

void loop()
{
  server.handleClient();
}

void handleRoot()
{
  String html = "<html><head><style>";
  html += "body { font-family: Arial, Helvetica, sans-serif; text-align: center; margin: 50px; }";
  html += "h1 { color: #3498db; }";
  html += "button { background-color: #3498db; color: white; padding: 10px 20px; font-size: 16px; text-align: center; text-decoration: none; display: inline-block; margin: 4px 2px; cursor: pointer; border-radius: 4px; }";
  html += "</style></head><body>";
  html += "<h1>Stepper Motor Control</h1>";
  html += "<a href=\"/FWD1\"><button>Start Motor 1</button></a>";
  html += "<a href=\"/FWD2\"><button>Start Motor 2</button></a>";
  html +="<br>";
  html += "<a href=\"/BWD1\"><button>Back Motor 1</button></a>";
  html += "<a href=\"/BWD2\"><button>Back Motor 2</button></a>";
  html +="<br>";
  html += "<a href=\"/RST1\"><button>Reset Motor 1</button></a>";
  html += "<a href=\"/RST2\"><button>Reset Motor 2</button></a>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void startMotor1()
{
  if(currentPositionMotor1 >= maxRounds)
  {
    server.sendHeader("Location", "/");
    server.send(302, "text/plain", "Motor 1 reached end limit");
    Serial.println("motor 1: Reached end limit");
    return;
  }
  moveMotorForward(dirPinMotor1, stepPinMotor1, stepsPerRevolution * 1);
  currentPositionMotor1 += 1;
  Serial.println("motor 1: 1 round");
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "Motor 1 started");
}

void startMotor2()
{
  if(currentPositionMotor2 >= maxRounds)
  {
    server.sendHeader("Location", "/");
    server.send(302, "text/plain", "Motor 2 reached end limit");
    Serial.println("motor 2: Reached end limit");
    return;
  }
  moveMotorForward(dirPinMotor2, stepPinMotor2, stepsPerRevolution * 1);
  currentPositionMotor2 += 1; 
  Serial.println("motor 2: 1 round");
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "Motor 2 started");
}

void backMotor1()
{
  if(digitalRead(irSensorPinMotor1) == LOW)
  {
    server.sendHeader("Location", "/");
    server.send(302, "text/plain", "Motor 1 at intial position");
    Serial.println("motor 1: intial position");
    return;
  }
  moveMotorBackward(dirPinMotor1, stepPinMotor1, stepsPerRevolution * 1);
  currentPositionMotor1 -= 1;
  Serial.println("motor 1: -1 round");
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "Motor 1 backed");
}

void backMotor2()
{
  if(digitalRead(irSensorPinMotor2) == LOW)
  {
    server.sendHeader("Location", "/");
    server.send(302, "text/plain", "Motor 2 at intial position");
    Serial.println("motor 2: at intial positon");
    return;
  }
  moveMotorBackward(dirPinMotor2, stepPinMotor2, stepsPerRevolution * 1);
  currentPositionMotor2 -= 1; 
  Serial.println("motor 2: -1 round");
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "Motor 2 backed");
}

void rstMotor1()
{
  calibrateMotorPosition(dirPinMotor1, stepPinMotor1, irSensorPinMotor1);
  currentPositionMotor1=0;
  Serial.println("motor 1: Reset");
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "Motor 1 reset");
}

void rstMotor2()
{
  calibrateMotorPosition(dirPinMotor2, stepPinMotor2, irSensorPinMotor2); 
  currentPositionMotor2=0;
  Serial.println("motor 2: Reset");
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "Motor 2 reset");
}

void calibrateMotorPosition(int dirPin, int stepPin, int irSensorPin)
{
  // Move the motor backward until the IR sensor is triggered
  Serial.println("Inside calibration ");
  digitalWrite(dirPin, LOW);
  while (!digitalRead(irSensorPin) == LOW)
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(1000);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(1000);
    Serial.println("Motor going to initial position ");
  }
}

void moveMotorForward(int dirPin, int stepPin, int steps)
{
  digitalWrite(dirPin, HIGH);
  for (int i = 0; i < steps; i++)
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(1000);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(1000);
  }  
}

void moveMotorBackward(int dirPin, int stepPin, int steps)
{
  digitalWrite(dirPin, LOW);
  for (int i = 0; i < steps; i++)
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(1000);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(1000);
  }  
}
