#include <WiFi.h>
#include <esp_now.h>
#include <Wire.h>
#include <math.h>
#include <esp_wifi.h>   // ⭐ REQUIRED

#define MPU 0x68
#define SDA_PIN 21
#define SCL_PIN 22

uint8_t receiverMAC[] = {0xEC,0xE3,0x34,0x14,0x80,0xBC};

char command = 'S';

// FILTER + CONTROL
float activateAngle = 30;
float activeangleLR = 40;
float stopAngle = 25;
float alpha = 0.2;

float pitchF = 0;
float rollF = 0;

String state = "STOP";


void OnDataSent(const wifi_tx_info_t *info, esp_now_send_status_t status) {
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "OK" : "FAIL");
}

void setup() {
  Serial.begin(115200);

  Wire.begin(SDA_PIN, SCL_PIN);

  // Wake MPU
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  WiFi.mode(WIFI_STA);

  esp_now_init();

  esp_now_register_send_cb(OnDataSent);

  esp_now_peer_info_t peer = {};
  memcpy(peer.peer_addr, receiverMAC, 6);
  peer.channel = 0;
  peer.encrypt = false;

  esp_now_add_peer(&peer);

  Serial.println("🚀 Transmitter Ready");
}

void loop() {

  int16_t ax, ay, az;

  Wire.beginTransmission(MPU);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true);

  ax = Wire.read()<<8 | Wire.read();
  ay = Wire.read()<<8 | Wire.read();
  az = Wire.read()<<8 | Wire.read();

  float AccX = ax / 16384.0;
  float AccY = ay / 16384.0;
  float AccZ = az / 16384.0;

  float pitch = atan2(-AccX, sqrt(AccY*AccY + AccZ*AccZ)) * 180/PI;
  float roll  = atan2(AccY, AccZ) * 180/PI;

  pitchF = alpha*pitch + (1-alpha)*pitchF;
  rollF  = alpha*roll  + (1-alpha)*rollF;

  if (state == "STOP") {
    if (pitchF < -activateAngle) { command='B'; state="F"; }
    else if (pitchF > activateAngle) { command='F'; state="B"; }
    else if (rollF > activeangleLR) { command='R'; state="R"; }
    else if (rollF < -activeangleLR) { command='L'; state="L"; }
  } else {
    if (abs(pitchF)<stopAngle && abs(rollF)<stopAngle) {
      command='S';
      state="STOP";
    }
  }

  esp_now_send(receiverMAC, (uint8_t*)&command, sizeof(command));

  Serial.print("Cmd: ");
  Serial.print(command);
  // Serial.print("  Pitch:");
  // Serial.print(pitchF);
  // Serial.print("  Roll:");
  // Serial.println(rollF);

  delay(50);
}
