#define SENSOR_COUNT 5

// Pins for the 5 TCRT5000 sensors
const int sensorPins[SENSOR_COUNT] = {2, 3, 4, 5, 6};

// Motor control pins
const int motorLeftForward = 9;
const int motorLeftBackward = 10;
const int motorRightForward = 11;
const int motorRightBackward = 12;

// PWM pins for motor speed control
const int motorLeftSpeed = 3;
const int motorRightSpeed = 5;

// Sensor readings
int sensorValues[SENSOR_COUNT];

// Speed values
int baseSpeed = 150;  // Base speed of the motors
int maxSpeed = 255;   // Maximum speed of the motors

// Last known position of the line
int lastKnownPosition = 2;  // Start with the assumption that the line is in the middle

void setup() {
  Serial.begin(9600);

  // Initialize sensor pins @@
  for (int i = 0; i < SENSOR_COUNT; i++) {
    pinMode(sensorPins[i], INPUT);
  }

  // Initialize motor control pins
  pinMode(motorLeftForward, OUTPUT);
  pinMode(motorLeftBackward, OUTPUT);
  pinMode(motorRightForward, OUTPUT);
  pinMode(motorRightBackward, OUTPUT);

  // Initialize motor speed control pins
  pinMode(motorLeftSpeed, OUTPUT);
  pinMode(motorRightSpeed, OUTPUT);
}

void loop() {
  // Read sensor values
  for (int i = 0; i < SENSOR_COUNT; i++) {
    sensorValues[i] = digitalRead(sensorPins[i]);
  }

  // Determine the position of the line
  int linePosition = getLinePosition();

  // Control motors based on line position
  if (linePosition == -1) {
    // Line is lost, adjust based on last known position
    if (lastKnownPosition < 2) {
      // Last known position was to the left
      turnRightSlowly();
    } else if (lastKnownPosition > 2) {
      // Last known position was to the right
      turnLeftSlowly();
    } else {
      // Last known position was in the middle, stop
      stopMoving();
    }
  } else {
    // Line is detected, move accordingly
    if (linePosition < 2) {
      // Turn left
      moveLeft();
    } else if (linePosition > 2) {
      // Turn right
      moveRight();
    } else {
      // Move forward
      moveForward();
    }
    // Update last known position
    lastKnownPosition = linePosition;
  }

  delay(100);
}

int getLinePosition() {
  int position = 0;
  int activeSensors = 0;

  for (int i = 0; i < SENSOR_COUNT; i++) {
    if (sensorValues[i] == LOW) {  // Assuming LOW means line detected
      position += i;
      activeSensors++;
    }
  }

  if (activeSensors > 0) {
    return position / activeSensors;
  } else {
    return -1;  // Line is lost
  }
}

void moveForward() {
  setMotorSpeed(baseSpeed, baseSpeed);
  digitalWrite(motorLeftForward, HIGH);
  digitalWrite(motorLeftBackward, LOW);
  digitalWrite(motorRightForward, HIGH);
  digitalWrite(motorRightBackward, LOW);
}

void moveLeft() {
  setMotorSpeed(baseSpeed / 2, baseSpeed);  // Reduce left motor speed
  digitalWrite(motorLeftForward, HIGH);
  digitalWrite(motorLeftBackward, LOW);
  digitalWrite(motorRightForward, HIGH);
  digitalWrite(motorRightBackward, LOW);
}

void moveRight() {
  setMotorSpeed(baseSpeed, baseSpeed / 2);  // Reduce right motor speed
  digitalWrite(motorLeftForward, HIGH);
  digitalWrite(motorLeftBackward, LOW);
  digitalWrite(motorRightForward, HIGH);
  digitalWrite(motorRightBackward, LOW);
}

void turnLeftSlowly() {
  setMotorSpeed(baseSpeed / 2, baseSpeed);
  digitalWrite(motorLeftForward, LOW);
  digitalWrite(motorLeftBackward, HIGH);
  digitalWrite(motorRightForward, HIGH);
  digitalWrite(motorRightBackward, LOW);
}

void turnRightSlowly() {
  setMotorSpeed(baseSpeed, baseSpeed / 2);
  digitalWrite(motorLeftForward, HIGH);
  digitalWrite(motorLeftBackward, LOW);
  digitalWrite(motorRightForward, LOW);
  digitalWrite(motorRightBackward, HIGH);
}

void stopMoving() {
  setMotorSpeed(0, 0);
  digitalWrite(motorLeftForward, LOW);
  digitalWrite(motorLeftBackward, LOW);
  digitalWrite(motorRightForward, LOW);
  digitalWrite(motorRightBackward, LOW);
}

void setMotorSpeed(int leftSpeed, int rightSpeed) {
  analogWrite(motorLeftSpeed, constrain(leftSpeed, 0, maxSpeed));
  analogWrite(motorRightSpeed, constrain(rightSpeed, 0, maxSpeed));
}
