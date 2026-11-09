#include <Wire.h>
#include <MPU9250.h>
#include "BluetoothSerial.h" // <-- NEW: Include the Bluetooth library

// --- Create a Bluetooth Serial object ---
BluetoothSerial SerialBT; // <-- NEW

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

// --- MPU9250 globals ---
#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22
#define PI 3.1415926535

bfs::Mpu9250 IMU(&Wire, bfs::Mpu9250::I2C_ADDR_PRIM);
int status;

void setup() {
  // Start the standard serial for debugging if you need it
  Serial.begin(115200);

  // --- Start Bluetooth Serial ---
  SerialBT.begin("Smart_Cricket_Bat"); // <-- NEW: Give your Bluetooth device a name
  Serial.println("Bluetooth device is ready. You can now pair with it!");
  
  // Use SerialBT for the main output from now on
  SerialBT.println("Smart Bat: First Trigger Detection Mode");
  SerialBT.println("Waiting for impact...");

  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  status = IMU.Begin();
  if (status < 0) {
    Serial.println("IMU initialization failed! Check wiring, power.");
    SerialBT.println("IMU initialization failed! Check wiring, power."); // <-- NEW
    while (1) {}
  }
  IMU.ConfigAccelRange(bfs::Mpu9250::ACCEL_RANGE_16G);
  IMU.ConfigGyroRange(bfs::Mpu9250::GYRO_RANGE_2000DPS);
  IMU.ConfigDlpfBandwidth(bfs::Mpu9250::DLPF_BANDWIDTH_41HZ);
  IMU.ConfigSrd(4);
}

void loop() {
  topVal = analogRead(SENSOR_TOP_PIN);
  middleVal = analogRead(SENSOR_MIDDLE_PIN);
  bottomVal = analogRead(SENSOR_BOTTOM_PIN);

  if (!impactDetected) {
    if (topVal > TRIGGER_THRESHOLD) {
      SerialBT.println("-------------------------");       // <-- Changed to SerialBT
      SerialBT.println("💥 IMPACT DETECTED! 💥");          // <-- Changed to SerialBT
      SerialBT.println("Region: Top (Near Handle)");     // <-- Changed to SerialBT
      impactDetected = true;
    } else if (middleVal > TRIGGER_THRESHOLD) {
      SerialBT.println("-------------------------");       // <-- Changed to SerialBT
      SerialBT.println("💥 IMPACT DETECTED! 💥");          // <-- Changed to SerialBT
      SerialBT.println("Region: Middle (Sweet Spot)");   // <-- Changed to SerialBT
      impactDetected = true;
    } else if (bottomVal > TRIGGER_THRESHOLD) {
      SerialBT.println("-------------------------");       // <-- Changed to SerialBT
      SerialBT.println("💥 IMPACT DETECTED! 💥");          // <-- Changed to SerialBT
      SerialBT.println("Region: Bottom (Toe)");          // <-- Changed to SerialBT
      impactDetected = true;
    }
  }

  if (impactDetected) {
    // Note: SerialBT.printf() works just like Serial.printf()
    SerialBT.printf("Sensor Values at trigger -> Top: %d | Middle: %d | Bottom: %d\n", topVal, middleVal, bottomVal); // <-- Changed to SerialBT

    // === MPU9250 angle and speed reporting ===
    if (IMU.Read()) {
      float ax = IMU.accel_x_mps2() / 9.81f;
      float ay = IMU.accel_y_mps2() / 9.81f;
      float az = IMU.accel_z_mps2() / 9.81f;
      float gyro_y_deg = IMU.gyro_y_radps() * (180.0f / PI);

      // Simple Y-axis tilt (pitch)
      float pitch_deg = atan2(-ax, sqrt(ay*ay + az*az)) * 180.0f / PI;

      SerialBT.printf("Bat angle (pitch): %.2f deg\n", pitch_deg);       // <-- Changed to SerialBT
      SerialBT.printf("Bat Y-axis rotation speed: %.2f deg/s\n", gyro_y_deg); // <-- Changed to SerialBT
    } else {
      SerialBT.println("MPU9250 not ready at impact."); // <-- Changed to SerialBT
    }

    SerialBT.println("-------------------------"); // <-- Changed to SerialBT
    delay(1000);
    impactDetected = false;
    SerialBT.println("\nWaiting for next impact..."); // <-- Changed to SerialBT
  }
}