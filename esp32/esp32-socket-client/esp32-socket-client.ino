#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>

Adafruit_MPU6050 mpu;

// Informações da rede WiFi.
const char* ssid = "yourssid";
const char* pass = "yourpass";
const char* host = "hostip";
const int16_t port = 8080;

// Moving consts.
const int STOPPED = 0;
const int MOVING = 1;

// INTENSITY
const int WALK_SPEED= 0;
const int RUN_SPEED = 1;

// Direction consts.
const int MOV_CENTER = 0;
const int MOV_FORWARD = 1;
const int MOV_BACKWARDS = 2;
// Tilt.
const int CENTER = 0;
const int LEFT = 1;
const int RIGHT = 2;


// Helper variables.
float accelXOldValue = 0.0;
float accelXCurrValue = 0.0;
float accelXMoveThreshold = 2.0;

float gyroXCurrValue = 0.0;
float gyroXOldValue = 0.0;
float gyroXMoveThreshold = 0.2;


// Result to socket.
int currentMovement = STOPPED;
int currentSpeed = WALK_SPEED;
int currentDirection = MOV_FORWARD;
int currentTilt = CENTER;

int read_delay = 100;

void setup(void) {
  Serial.begin(9600);

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  // Connecting to wifi...
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected!");
  Serial.println("nConnecting to socket!");
  connectSocket();
  Serial.println("Connected!");
  Serial.println("Starting main loop!");
  Serial.println("");
  delay(100);
}

void loop() {
  if (client.connected()) {
    mpu_read();
    HTTPClient http;
    String currMov = String(currentMovement);
    String currSpeed = String(currentSpeed);
    String currDir = String(currentDirection);
    String currTilt = String(currentTilt);
    String result = String(currMov + currSpeed + currDir + currTilt);
    Serial.println(result);
    client.print(result);
  } else {
    client.stop();
    connectSocket();
  }
  Serial.println("----");
  delay(read_delay);
}

void connectSocket() {
  while (!client.connect(host, port)) {
    Serial.println("\nConnection to host failed");
    delay(500);
  }
}

void mpu_read() {
  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  accelXCurrValue = abs(a.acceleration.x);
  float accelXDiff = accelXCurrValue - accelXOldValue;
  float accelXDiffAbs = abs(accelXDiff);

  gyroXCurrValue = abs(g.gyro.x);
  float gyroXDiff = gyroXCurrValue - gyroXOldValue;
  float gyroXDiffAbs = abs(gyroXDiff);
  
  if (gyroXDiffAbs > gyroXMoveThreshold && accelXDiffAbs > accelXMoveThreshold) {
    currentMovement = MOVING;
  } else {
    currentMovement = STOPPED;
  }

//  if (a.acceleration.z > 3.) {
//    currentDirection = MOV_BACKWARDS;
//  } else {
//    currentDirection = MOV_FORWARD;
//  }

  gyroXOldValue = gyroXCurrValue;
  accelXOldValue = accelXCurrValue;
}