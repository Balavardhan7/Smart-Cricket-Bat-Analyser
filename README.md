# Smart Cricket Bat Impact Analyzer 🏏

[![Open Source](https://img.shields.io/badge/Open-Source-blue)](https://github.com/your-repo/smart-cricket-bat)
[![ESP32](https://img.shields.io/badge/ESP32-Microcontroller-green)](https://www.espressif.com/en/products/socs/esp32)
[![C++](https://img.shields.io/badge/C++-Programming-red)](https://isocpp.org/)
[![OpenCode](https://img.shields.io/badge/OpenCode-Community-orange)](https://opencode.dev)

## Project Overview

The Smart Cricket Bat Impact Analyzer is an IoT-based sports analytics tool designed to enhance cricket training through precise impact and motion analysis. This project leverages an ESP32 microcontroller, an MPU9250 IMU, and three piezoelectric sensors to detect the exact impact point on a cricket bat and analyze the bat's swing mechanics.

## Features

- **Impact Detection**: Identifies the impact region (Top, Middle/Sweet Spot, Bottom/Toe) on the bat.
- **Swing Analysis**: Measures the bat's pitch angle and Y-axis rotation speed at the moment of impact.
- **Dual Modes**:
  - **Bluetooth Mode**: Transmits data via Bluetooth Serial.
  - **Web Server Mode**: Serves real-time data through a web interface.

## Hardware Architecture

| Component             | Details                              |
|----------------------|----------------------------------------|
| Microcontroller        | ESP32                                  |
| IMU                  | MPU9250 (I2C: SDA - GPIO 21, SCL - GPIO 22) |
| Piezoelectric Sensors| Top: GPIO 33, Middle: GPIO 35, Bottom: GPIO 34 |
| Cricket Bat           | N/A                                    |

## Software Installation & Setup

### Prerequisites

- [Arduino IDE](https://www.arduino.cc/en/software)
- ESP32 Board Package
- Required Libraries:
  - `MPU9250` by bfs
  - `ESPAsyncWebServer`
  - `Arduino_JSON`

### Installation Steps

1. **Install Arduino IDE**:
   - Download and install the Arduino IDE from the [official website](https://www.arduino.cc/en/software).

2. **Install ESP32 Board Package**:
   - Open Arduino IDE.
   - Go to `File > Preferences`.
   - Add the following URL to "Additional Board Manager URLs":
     ```
     https://dl.espressif.com/dl/package_esp32_index.json
     ```
   - Go to `Tools > Board > Boards Manager`.
   - Search for "esp32" and install the latest version.

3. **Install Required Libraries**:
   - Go to `Sketch > Include Library > Manage Libraries`.
   - Search for and install:
     - `MPU9250` by bfs
     - `ESPAsyncWebServer`
     - `Arduino_JSON`

4. **Upload SPIFFS Data**:
   - Install the [ESP32 File System](https://github.com/me-no-dev/arduino-esp32fs-plugin) plugin for Arduino IDE.
   - Upload the data files (`index.html`, `image.png`) to the ESP32 using the plugin.

5. **Configure Wi-Fi Credentials**:
   - Open `website_code.ino`.
   - Update the following lines with your Wi-Fi credentials:
     ```cpp
     const char* ssid = "your_SSID";
     const char* password = "your_PASSWORD";
     ```

## Usage Instructions

### Bluetooth Mode

1. Upload `bluetooth_code.ino` to the ESP32.
2. Pair your device with "Smart_Cricket_Bat" via Bluetooth.
3. Open a serial terminal and wait for impact data.

### Web Server Mode

1. Upload `website_code.ino` to the ESP32.
2. Connect to the ESP32's IP address in a web browser.
3. The web interface will display real-time data and highlight the impact region.

## Contributing Guidelines

We welcome contributions from the community! To contribute:

1. Fork the repository.
2. Create a feature branch: `git checkout -b feature/YourFeature`.
3. Commit your changes: `git commit -m "Add some feature"`.
4. Push to the branch: `git push origin feature/YourFeature`.
5. Open a pull request.

For more details, please read our [CONTRIBUTING.md](CONTRIBUTING.md).

## License & Credits

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

### Author

- **Andaluri Balavardhan** - National Institute of Technology, Warangal
