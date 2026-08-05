# 🌱 IoT-Enabled Smart Sugarcane Farming System

An IoT-enabled smart irrigation and environmental monitoring system developed using **STM32**, **ESP32**, **LoRa (433 MHz)**, **Supabase**, and **Blynk IoT**. The system continuously monitors field conditions, stores sensor data in the cloud, provides real-time visualization through a web dashboard, and enables remote irrigation control through a mobile application.

---

# 📖 Project Overview

Efficient water management is one of the major challenges in modern agriculture. This project addresses that challenge by deploying multiple wireless sensor nodes across a sugarcane field to monitor environmental conditions and assist farmers in irrigation management.

The system consists of **three STM32-based sensor nodes**, each equipped with environmental sensors and a LoRa communication module. Sensor readings are transmitted wirelessly to an **ESP32 gateway**, which displays the information locally, uploads it to the cloud, and enables remote monitoring and irrigation control.

---

# 🏗️ System Architecture

<p align="center">
<img src="Images/system_architecture.png" width="900">
</p>

---

# ⚙️ Hardware Components

## Sensor Nodes (3)

Each sensor node contains:

- STM32 Nucleo F401RE
- SX1278 LoRa Module (433 MHz)
- DHT11 Temperature & Humidity Sensor
- Capacitive Soil Moisture Sensor
- Rain Sensor
- AMS1117 Voltage Regulator
- Li-ion Battery

---

## Gateway Unit

The central gateway consists of:

- ESP32 Development Board
- SX1278 LoRa Module
- 16×2 I2C LCD Display
- 4-Channel Relay Module
- Water Pump
- Three Solenoid Valves

---

# 💻 Software & Technologies

### Embedded Development

- STM32CubeIDE
- STM32 HAL Library
- Arduino IDE

### Cloud & IoT

- Supabase Database
- Blynk IoT Platform

### Communication

- LoRa (433 MHz)

### Version Control

- GitHub

---

# 🔄 Working Principle

1. Environmental sensors continuously monitor field conditions.
2. STM32 processes sensor data.
3. Data is transmitted wirelessly using LoRa.
4. ESP32 receives data from all sensor nodes.
5. Sensor readings are displayed on the LCD.
6. ESP32 uploads data to the cloud database.
7. The web dashboard displays live sensor readings and historical trends.
8. Users can remotely control irrigation using the Blynk mobile application.
9. Relay outputs operate the pump and solenoid valves.

---

# 📊 Web Dashboard

The project includes a custom web dashboard for real-time visualization of environmental data.

## Landing Page

<p align="center">
<img src="Images/landing_page.png" width="900">
</p>

---

## Dashboard Overview

The dashboard provides:

- Active sensor node status
- Average soil moisture
- Average temperature
- Alert monitoring
- Soil moisture comparison
- Node status table

<p align="center">
<img src="Images/dashboard_overview.png" width="900">
</p>

---

## Node Monitoring

Each sensor node has a dedicated monitoring page displaying:

- Soil Moisture
- Temperature
- Humidity
- Rain Status
- Historical sensor graphs

<p align="center">
<img src="Images/node_details.png" width="900">
</p>

---

## Node Summary

The dashboard also provides a quick summary of all deployed nodes.

<p align="center">
<img src="Images/node_summary.png" width="900">
</p>

---

# 📱 Mobile Application

The system integrates with the **Blynk IoT** mobile application for remote monitoring and irrigation control.

Features include:

- Live environmental monitoring
- Pump control
- Individual valve control
- Remote access through the Internet
- Easy-to-use mobile interface

<p align="center">
<img src="Images/blynk_dashboard.jpg" width="350">
</p>

---

# 📷 Hardware Prototype

## Sensor Nodes

Three STM32-based wireless sensor nodes deployed for field monitoring.

<p align="center">
<img src="Images/hardware_setup.jpg" width="850">
</p>

---

## ESP32 Gateway

Central gateway responsible for LoRa communication, cloud connectivity, LCD display, and irrigation control.

<p align="center">
<img src="Images/esp32_gateway.jpg" width="500">
</p>

---

# ✨ Key Features

- 🌱 Multi-node environmental monitoring
- 📡 Long-range LoRa communication (433 MHz)
- 📱 Remote irrigation control through Blynk
- 🌐 Real-time cloud dashboard
- ☁️ Cloud data storage using Supabase
- 💧 Pump and valve automation support
- 📊 Historical sensor visualization
- 🔋 Low-power sensor nodes
- 📟 LCD-based local monitoring
- 📡 Wireless data transmission

---

# 📂 Repository Structure

```text
IoT-Enabled-Smart-Sugarcane-Farming-System
│
├── Documentation/
│
├── ESP32_Gateway/
│
├── STM32_Node1/
├── STM32_Node2/
├── STM32_Node3/
│
├── Images/
│   ├── system_architecture.png
│   ├── landing_page.png
│   ├── dashboard_overview.png
│   ├── node_summary.png
│   ├── node_details.png
│   ├── hardware_setup.jpg
│   ├── esp32_gateway.jpg
│   └── blynk_dashboard.jpg
│
├── LICENSE
└── README.md
```

---

# 🚀 Future Enhancements

- Automatic irrigation based on soil moisture threshold
- Weather forecast integration
- Solar-powered sensor nodes
- AI-based irrigation recommendations
- Mobile notifications
- Fertilizer recommendation system
- Predictive crop analytics

---

# 📜 License

This project is licensed under the MIT License.

---

# 👨‍💻 Author

## Suhas Reddy

**B.E. Electronics and Communication Engineering**

M. S. Ramaiah Institute of Technology

---

⭐ If you found this project useful, consider giving the repository a **Star**.
