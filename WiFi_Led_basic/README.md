# WiFi Controlled LED using ESP8266

This project demonstrates how to control an LED over WiFi using an ESP8266 microcontroller. The ESP8266 acts as a web server, allowing you to turn the LED on and off remotely through a simple web interface.

## Requirements
- ESP8266 Board
- LED
- Arduino IDE
- ESP8266WiFi library
- ESP8266WebServer library

## Circuit Diagram
Connect an LED to pin D2 of the ESP8266. Make sure to use an appropriate current-limiting resistor if required.

## Setup
1. Open the Arduino IDE.
2. Install the ESP8266 board package.
3. Install the required libraries (`ESP8266WiFi` and `ESP8266WebServer`).
4. Upload the provided code to your ESP8266 board.

## Usage
- Connect to the WiFi network named "WiFi-Led" with the password "123456789".
- Open a web browser and navigate to the IP address assigned to the ESP8266.
- Visit the IP address assigned to the ESP8266 in a web browser (in general 192.168.4.1 it may vary so use the serial monitor to print the local IP)
- You will see a simple control panel page having nothing.
- In the URL <IP>/on to turn the LED on.
- In the URL <IP>/off to turn the LED off.

## Notes
- Ensure that the LED is connected to the correct pin (`LedPin`) as specified in the code.
- Customize the WiFi SSID and password in the code according to your network settings.
