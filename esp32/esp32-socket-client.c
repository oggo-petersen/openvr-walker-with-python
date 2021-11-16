#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>

const int MPU_addr=0x68;
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
int16_t xVal = 0;
int16_t oldXVal = 0;
int16_t diffThreshold = 4000;
bool walking = false;

// Wifi network.
const char* ssid = "yourssid";
const char* pass = "yourpassword";
const char* host = "192.168.0.0";
const int16_t port = 8080;

WiFiClient client;

void setup(){
  Serial.begin(9600);
  // MPU Initialization.
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  // Connecting to wifi...
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.println("\nConnecting to socket!");
  connectSocket();
  Serial.println("\nConnected!");
  Serial.println("\nStarting main loop!\n");
}

void loop(){
  if (client.connected()) {
    mpu_read();
    HTTPClient http;
    if (walking) {
      Serial.println("T");
      client.print("T");
    } else {
      Serial.println("F");
      client.print("F");  
    }    
  } else {
    client.stop();
    connectSocket();
  }
}

void connectSocket() {
  while (!client.connect(host, port)) {
    Serial.println("\nConnection to host failed");
    delay(500);
  }
}

void mpu_read(){
 Wire.beginTransmission(MPU_addr);
 Wire.write(0x3B);
 Wire.endTransmission(false);
 Wire.requestFrom(MPU_addr,14,true);
 AcX=Wire.read()<<8|Wire.read();
 AcY=Wire.read()<<8|Wire.read();
 AcZ=Wire.read()<<8|Wire.read();
 GyX=Wire.read()<<8|Wire.read();
 GyY=Wire.read()<<8|Wire.read();
 GyZ=Wire.read()<<8|Wire.read();
 xVal = AcX;
 if (xVal != oldXVal) {
    int remainder = xVal - oldXVal;
    if (remainder < 0) {
      remainder = remainder * -1;
    }

    walking = remainder > diffThreshold;
    oldXVal = xVal;
 }
 delay(100);
}