# Semi-Autonomous Rover with Human-Kinetic Interface

A semi-autonomous rover controlled through human hand movements using an MPU6050 IMU and ESP32, with autonomous safety mechanisms for obstacle and cliff detection.

## Overview

The system uses a smart glove to capture hand orientation and wirelessly transmit motion commands to the rover. The rover combines human control with sensor-based safety mechanisms to prevent unsafe movement.

## Key Features

- Hand-motion based rover control using MPU6050
- ESP-NOW wireless communication between ESP32 modules
- PWM-based motor speed and direction control
- Ultrasonic and IR-based hazard detection
- Safety-veto mechanism for autonomous hazard avoidance
## Project Demonstration

[▶️ Watch the Project Demonstration](./project_demo.mp4)
## Hardware

- ESP32 × 2
- MPU6050 IMU
- Ultrasonic sensor
- IR sensor
- Motor driver
- DC motors
- LM358-based battery monitor
- 3S Li-ion battery
