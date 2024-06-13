// Define motor control pins
const int IN1 = 8;
const int IN2 = 9;
const int ENA = 10;

void setup() {
  // Set motor control pins as outputs
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);

  // Initialize motor state
  stopMotor();
}

void loop() {
  // Example motor control
  forward(255);  // Move forward at full speed
  delay(2000);   // Run for 2 seconds
  stopMotor();   // Stop motor
  delay(1000);   // Wait for 1 second
  
  backward(128); // Move backward at half speed
  delay(2000);   // Run for 2 seconds
  stopMotor();   // Stop motor
  delay(1000);   // Wait for 1 second
}

void forward(int speed) {
  // Set motor direction to forward
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  
  // Set motor speed
  analogWrite(ENA, speed);
}

void backward(int speed) {
  // Set motor direction to backward
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  
  // Set motor speed
  analogWrite(ENA, speed);
}

void stopMotor() {
  // Stop the motor
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  
  // Set speed to 0
  analogWrite(ENA, 0);
}
