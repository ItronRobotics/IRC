# ESP8266 Wi-Fi Stepper Motor Control
This project allows you to control two stepper motors connected to an ESP8266 microcontroller board over Wi-Fi using a web interface.

## Features
- Control two stepper motors forwards, backwards and reset.
- Reset motor positions based on infrared (IR) sensor readings.
- Web interface for easy control from any device with a web browser.
- Status messages logged on the serial monitor.

## Hardware Requirements
1. ESP8266 microcontroller board (e.g., NodeMCU)
2. Two stepper motors
3. Two stepper motor driver boards (e.g., A4988)
4. Two infrared sensors
5. Breadboard and jumper wires

## Usage
- Upload the code to your ESP8266 board using the Arduino IDE.
- Connect to the Wi-Fi network created by the ESP8266 ("BionicControl" with password "123456789").
- Open a web browser on any device connected to the Wi-Fi network and enter the IP address displayed on the serial monitor in the address bar.
- The web interface will display buttons to control each motor forwards, backwards, and reset.

## Additional Notes
- The stepsPerRevolution variable defines the number of steps required for the motor to complete one full revolution. You may need to adjust this value based on your specific stepper motor.
- The maxRounds variable limits the number of rotations a motor can make before automatically stopping in the forward direction to prevent overrunning.