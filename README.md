🌞 IoT-Based Renewable Energy Monitoring System for Microgrid ⚡

An end-to-end hardware + IoT solution for real-time monitoring of a renewable energy microgrid using ESP32.
The system measures solar generation, battery storage, load consumption, power, and cumulative energy through a live web dashboard hosted on ESP32.

📌 Project Overview

This project demonstrates a small-scale solar microgrid with IoT-based monitoring capabilities.
It integrates renewable energy hardware with embedded firmware and a web interface to visualize real-time electrical parameters.

Key objectives:

Monitor renewable energy generation

Track battery charging & discharging

Measure load consumption

Calculate real-time power and total energy usage

🔧 Hardware Components

ESP32 Development Board

Solar Panel (6V, 2.5W)

Li-ion Batteries (×4)

Battery Holders (×2)

TP4056 Li-ion Charging Module

7805 Voltage Regulator

Voltage Sensors (×2)

Current Sensor

DC Motor (Load)

Switches & PCB

ESP32 USB Cable

💻 Software & Technologies

Embedded C/C++ (Arduino Framework)

ESP32 WebServer

HTML, CSS, JavaScript

WiFi-based real-time monitoring

JSON data transfer

⚙ Working Principle

The solar panel generates power based on light intensity.

TP4056 module ensures safe charging of Li-ion batteries with over-charge & deep-discharge protection.

Battery supplies power to the DC motor (load).

ESP32 reads voltage and current values using ADC pins.

Power is calculated using P = V × I.

Energy consumption is computed in Watt-hours (Wh) using time-based integration.

All data is displayed on a real-time web dashboard accessible via ESP32’s local IP address.

📊 Features

✔ Real-time voltage & current monitoring
✔ Power & energy (Wh / kWh) calculation
✔ Live web dashboard hosted on ESP32
✔ Resettable energy counter
✔ Battery protection using TP4056
✔ Physical hardware implementation (not simulation)

🌐 Web Dashboard

Updates every 2 seconds

Displays:

Solar Voltage

Battery Voltage

Load Voltage

Current

Power (Solar & Load)

Total Energy Consumption

Accessible via browser using ESP32 local IP

📁 Repository Structure
📦 IoT-Based-Renewable-Energy-Monitoring-System-for-Microgrid
 ┣ 📂 Code
 ┃ ┗ ESP32_Firmware.ino
 ┣ 📂 Documentation
 ┃ ┣ Project_Report.pdf
 ┃ ┣ Research_Paper.pdf
 ┃ ┗ Presentation.pptx
 ┣ 📂 Images
 ┃ ┗ Hardware_Setup.jpg
 ┗ README.md

🎯 Applications

Smart Microgrids

Renewable Energy Systems

Energy Monitoring & Analytics

Educational & Research Projects

IoT-Based Power Management

🔮 Future Enhancements

Cloud data logging (Firebase / MQTT)

Mobile application support

MPPT charge controller integration

Remote monitoring over internet

Predictive energy analytics

🧑‍💻 Author

Vinit Takate
Mechanical Engineering | Renewable Energy | IoT Systems
