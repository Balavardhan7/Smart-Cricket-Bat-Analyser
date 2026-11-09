// --- Libraries ---
#include <Wire.h>
#include <MPU9250.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <Arduino_JSON.h>

// --- WiFi Credentials ---
const char* ssid = "itel P55 5G";     // 👈 CHANGE THIS
const char* password = "11111111"; // 👈 CHANGE THIS

// --- Piezo pins ---
const int SENSOR_TOP_PIN = 33;
const int SENSOR_MIDDLE_PIN = 35;
const int SENSOR_BOTTOM_PIN = 34;
const int TRIGGER_THRESHOLD = 400;

// --- Piezo globals ---
int topVal = 0;
int middleVal = 0;
int bottomVal = 0;
bool impactDetected = false;
unsigned long impactTimestamp = 0; // For non-blocking delay

// --- MPU9250 globals ---
#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22
#define PI 3.1415926535

bfs::Mpu9250 IMU(&Wire, bfs::Mpu9250::I2C_ADDR_PRIM);
int status;

// --- Web Server & Data Globals ---
AsyncWebServer server(80); // Create AsyncWebServer object on port 80

// Global variables to hold the last impact data for the web server
String lastRegion = "none";
float lastAngle = 0.0;
float lastSpeed = 0.0;

// --- Wi-Fi Setup ---
void setupWiFi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// --- Server Routes Setup ---
void setupServerRoutes() {
  // 1. Route for the root URL ("/") - serves index.html
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });

  // 2. Route for the image
  server.on("/image.png", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/image.png", "image/png");
  });

  // 3. Route for the /data endpoint (sends JSON)
  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request) {
    JSONVar jsonData;
    jsonData["region"] = lastRegion;
    jsonData["angle"] = lastAngle;
    jsonData["speed"] = lastSpeed;

    String jsonString = JSON.stringify(jsonData);
    request->send(200, "application/json", jsonString);
  });

  // 4. Handle Not Found
  server.onNotFound([](AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
  });
}

// --- Main Setup ---
void setup() {
  Serial.begin(115200);
  Serial.println("Smart Bat: Web Server Mode");

  // --- Start Wi-Fi ---
  setupWiFi();

  // --- Start SPIFFS ---
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // --- Start MPU9250 ---
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  status = IMU.Begin();
  if (status < 0) {
    Serial.println("IMU initialization failed! Check wiring, power.");
    while (1) {}
  }
  IMU.ConfigAccelRange(bfs::Mpu9250::ACCEL_RANGE_16G);
  IMU.ConfigGyroRange(bfs::Mpu9250::GYRO_RANGE_2000DPS);
  IMU.ConfigDlpfBandwidth(bfs::Mpu9250::DLPF_BANDWIDTH_41HZ);
  IMU.ConfigSrd(4);

  // --- Start Web Server ---
  setupServerRoutes();
  server.begin();
  Serial.println("HTTP server started.");
  Serial.println("Waiting for impact...");
}

// --- Main Loop ---
void loop() {
  
  // 1. Check for a new impact (if not in "cooldown")
  if (!impactDetected) {
    topVal = analogRead(SENSOR_TOP_PIN);
    middleVal = analogRead(SENSOR_MIDDLE_PIN);
    bottomVal = analogRead(SENSOR_BOTTOM_PIN);

    String detectedRegion = "";

    if (topVal > TRIGGER_THRESHOLD) {
      detectedRegion = "top";
    } else if (middleVal > TRIGGER_THRESHOLD) {
      detectedRegion = "middle";
    } else if (bottomVal > TRIGGER_THRESHOLD) {
      detectedRegion = "bottom";
    }

    // --- If an impact happened ---
    if (detectedRegion != "") {
      impactDetected = true;
      impactTimestamp = millis(); // Start the 1-second "cooldown"
      lastRegion = detectedRegion; // Save region for the web

      // --- Get IMU data AT THE MOMENT of impact ---
      if (IMU.Read()) {
        float ax = IMU.accel_x_mps2() / 9.81f;
        float ay = IMU.accel_y_mps2() / 9.81f;
        float az = IMU.accel_z_mps2() / 9.81f;
        float gyro_y_deg = IMU.gyro_y_radps() * (180.0f / PI);
        float pitch_deg = atan2(-ax, sqrt(ay * ay + az * az)) * 180.0f / PI;

        // Save data for the web
        lastAngle = pitch_deg;
        lastSpeed = gyro_y_deg;

        // --- Print to Serial Monitor ---
        Serial.println("-------------------------");
        Serial.println("💥 IMPACT DETECTED! 💥");
        if (lastRegion == "top") Serial.println("Region: Top (Near Handle)");
        else if (lastRegion == "middle") Serial.println("Region: Middle (Sweet Spot)");
        else Serial.println("Region: Bottom (Toe)");

        Serial.printf("Sensor Values -> Top: %d | Middle: %d | Bottom: %d\n", topVal, middleVal, bottomVal);
        Serial.printf("Bat angle (pitch): %.2f deg\n", lastAngle);
        Serial.printf("Bat Y-axis rotation speed: %.2f deg/s\n", lastSpeed);
        Serial.println("-------------------------");
        
      } else {
        Serial.println("MPU9250 not ready at impact.");
        // Reset values on a failed read
        lastAngle = 0; 
        lastSpeed = 0;
      }
    }
  }

  // 2. Check if "cooldown" period is over (non-blocking)
  // This replaces your "delay(1000)"
  if (impactDetected && (millis() - impactTimestamp > 1000)) {
    impactDetected = false; // Ready to detect the next impact
    Serial.println("\nWaiting for next impact...");
  }
  
  // Note: No 'delay()' is used in the main loop.
  // The Async web server handles client requests in the background.
}