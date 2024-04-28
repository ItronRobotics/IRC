# WiFi Controlled LED with Light Intensity Sensing using ESP8266

This project demonstrates how to control an LED over WiFi using an ESP8266 microcontroller while also sensing light intensity with an LDR (Light Dependent Resistor) sensor. The ESP8266 acts as a web server, allowing you to monitor the light intensity and remotely control the LED based on the sensed light level.

## Requirements
- LED
- LDR sensor
- ESP8266 Board
- Arduino IDE
- ESP8266WiFi library
- ESP8266WebServer library

## Circuit Diagram
Connect an LED to pin D2 of the ESP8266 and an LDR sensor to pin A0. Ensure to use appropriate current-limiting resistors if required.

## Setup
1. Open the Arduino IDE.
2. Install the ESP8266 board package.
3. Install the required libraries (`ESP8266WiFi` and `ESP8266WebServer`).
4. Upload the provided code to your ESP8266 board.

## Usage
- Connect to the WiFi network named "LED_Controller" with the password "password".
- Open a web browser and navigate to the IP address assigned to the ESP8266.
- Visit the IP address assigned to the ESP8266 in a web browser.
- You will see a webpage displaying the current light intensity and background color changes based on the light level.
- The LED connected to pin D2 will be controlled automatically based on the sensed light intensity.

## Notes
- Ensure that the LED is connected to the correct pin (`LedPin`) and the LDR sensor to the correct pin (`LdrPin`) as specified in the code.
- Customize the WiFi SSID and password in the code according to your network settings.
- Adjust the threshold value (`threshold`) according to your desired light intensity level for LED control.
