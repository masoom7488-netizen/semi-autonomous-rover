#include <WiFi.h>

#include <esp_now.h>

// MOTOR PINS

#define ENA 13

#define IN1 14

#define IN2 27

#define IN3 26

#define IN4 25

#define ENB 33

// SENSOR PINS

#define TRIG_PIN 32

#define ECHO_PIN 35

#define IR_PIN 34

char command = 'S';

char lastCommand = 'S';

int motorSpeed = 180;

int turn90Duration = 350;

// SAFETY TUNING

int obstacleDistanceLimit = 15; // Stop if object is closer than 15 cm

// RECEIVE CALLBACK

void OnDataRecv(const esp_now_recv_info *info, const uint8_t *data, int len) {

  command = *data;

  Serial.print("Received: ");

  Serial.println(command);

}

void setup() {

  Serial.begin(115200);

  delay(2000);

  Serial.println("ESP32 Receiver Booting...");

  // MOTOR PINS SETUP

  pinMode(IN1, OUTPUT);

  pinMode(IN2, OUTPUT);

  pinMode(IN3, OUTPUT);

  pinMode(IN4, OUTPUT);

  // SENSOR PINS SETUP

  pinMode(TRIG_PIN, OUTPUT);

  pinMode(ECHO_PIN, INPUT);

  pinMode(IR_PIN, INPUT);

  // PWM SETUP

  ledcAttach(ENA, 1000, 8);  

  ledcAttach(ENB, 1000, 8);

  stopMotors();

  // WIFI + ESPNOW

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {

    Serial.println("ESP-NOW INIT FAILED");

    return;

  }

  esp_now_register_recv_cb(OnDataRecv);

  Serial.println("🚀 Receiver Ready");

}

void loop() {

  // 1. ALWAYS CHECK SAFETY FIRST

  bool isSafe = checkFrontSafety();

  // 2. EXECUTE COMMANDS WITH OVERRIDE LOGIC

  if (command == 'F') {

    if (isSafe) {

      moveForward();

    } else {

      stopMotors(); // OVERRIDE: Danger detected, hit the brakes!

    }

  }

  else if (command == 'B') {

    moveBackward(); // Always allowed to reverse away from danger

  }

  else if (command == 'L') {

    turnLeft();     // Always allowed to turn away from danger

  }

  else if (command == 'R') {

    turnRight();    // Always allowed to turn away from danger

  }

  else {

    stopMotors();

  }

}

// ===== SAFETY LOGIC =====

bool checkFrontSafety() {

  // --- Check Drop (IR Sensor) ---

  // Note: Based on your prompt, we trigger danger if IR is NOT HIGH (i.e., LOW).

  // If your specific module works backwards, change this to: == HIGH

  bool dropDetected = (digitalRead(IR_PIN) == HIGH); 

  // --- Check Wall (Ultrasonic) ---

  digitalWrite(TRIG_PIN, LOW);

  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);

  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  // Timeout set to 30000us (approx 5 meters) to prevent the code from freezing

  long duration = pulseIn(ECHO_PIN, HIGH, 30000); 

  // Calculate distance in cm

  float distance = duration * 0.034 / 2;

  // Danger if distance is valid (> 0) AND closer than our limit

  bool obstacleDetected = (distance > 0 && distance < obstacleDistanceLimit);

  // If there is a drop OR an obstacle, it is NOT safe to move forward.

  if (dropDetected || obstacleDetected) {

    return false; // Danger!

  } 

  return true; // All clear

}


// ===== MOTOR FUNCTIONS =====

void moveBackward() {

  ledcWrite(ENA, motorSpeed);

  ledcWrite(ENB, motorSpeed);

  digitalWrite(IN1, HIGH);

  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);

  digitalWrite(IN4, LOW);

}

void moveForward() {

  ledcWrite(ENA, motorSpeed);

  ledcWrite(ENB, motorSpeed);

  digitalWrite(IN1, LOW);

  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);

  digitalWrite(IN4, HIGH);

}

void turnLeft() {

  ledcWrite(ENA, motorSpeed / 2.5);

  ledcWrite(ENB, motorSpeed / 1.5);

  digitalWrite(IN1, HIGH);

  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);

  digitalWrite(IN4, LOW);

}

void turnRight() {

  ledcWrite(ENA, motorSpeed / 1.2);

  ledcWrite(ENB, motorSpeed / 2.5);

  digitalWrite(IN1, HIGH);

  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);

  digitalWrite(IN4, HIGH);

}

void stopMotors() {

  ledcWrite(ENA, 0);

  ledcWrite(ENB, 0);

  digitalWrite(IN1, LOW);

  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);

  digitalWrite(IN4, LOW);

}
