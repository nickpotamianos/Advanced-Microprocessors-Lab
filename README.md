# Advanced Microprocessors Lab Projects

This repository contains the code and documentation for a series of laboratory exercises related to Advanced Microprocessors. The exercises explore various functionalities such as control systems, ADC configurations, interrupts, timers, and PWM for microcontroller-based systems.

## Table of Contents
- [Project Overview](#project-overview)
- [Lab Exercises](#lab-exercises)
  - [Lab 1: Traffic Light Control System](#lab-1-traffic-light-control-system)
  - [Lab 2: Room Navigation System](#lab-2-room-navigation-system)
  - [Lab 3: Fan Control Using PWM](#lab-3-fan-control-using-pwm)
  - [Lab 4: Moisture Sensor and Control Systems](#lab-4-moisture-sensor-and-control-systems)
- [Installation](#installation)
- [Usage](#usage)
- [Contributors](#contributors)
- [License](#license)

## Project Overview

The project focuses on implementing various systems using AVR microcontrollers. Each lab exercise covers different aspects of embedded systems, including ADCs, timers, interrupts, and PWM to build functional systems such as traffic lights, room navigation, fan control, and moisture sensors.

## Lab Exercises

### Lab 1: Traffic Light Control System
This exercise implements a traffic light control system with interrupts and timers for vehicles, pedestrians, and trams. The project involves:
- Managing traffic lights using predefined pins for vehicles and pedestrians.
- Using an interrupt-based system to switch between red and green lights.
- Extending functionality to control tram movements with periodic timing intervals.

**Key features:**
- Interrupt Service Routine (ISR) for pedestrian button press and timer expiry.
- Timer configuration to manage light durations and transitions.
- Efficient state management using flags and timers for simultaneous traffic light control&#8203;:contentReference[oaicite:0]{index=0}.

### Lab 2: Room Navigation System
The task is to design a navigation system for a robotic device that can move within a room. The system implements:
- Free-running ADC to monitor obstacles.
- Left and right turning logic based on sensor data.
- Room navigation using predefined obstacle detection.

**Key features:**
- ADC configuration to handle obstacle detection via sensors.
- Interrupts to handle turning logic based on sensor readings.
- Timer management to ensure accurate turning delays&#8203;:contentReference[oaicite:1]{index=1}.

### Lab 3: Fan Control Using PWM
This exercise controls a fan using Pulse Width Modulation (PWM) signals. The fan has three states: on, speed adjustment, and off. The system features:
- PWM signals to control the fan's blades and base rotation.
- Interrupts triggered by a button press to toggle between different states.
- Timer configuration to adjust the fan speed and manage LED indicators.

**Key features:**
- PWM configuration for independent control of fan blades and base.
- ISR to handle button press and switch between fan states.
- Timer to adjust fan speed dynamically&#8203;:contentReference[oaicite:2]{index=2}.

### Lab 4: Moisture Sensor and Control Systems
In this exercise, an ADC is used to monitor soil moisture levels, and different actions are triggered based on moisture thresholds:
- Moisture levels trigger corresponding LEDs for low or high moisture.
- Timers and PWM are used to manage watering and ventilation systems.
- Error detection when invalid input is provided by the user.

**Key features:**
- Continuous ADC monitoring of moisture levels with threshold comparisons.
- Timer-triggered watering and ventilation based on sensor readings.
- PWM for controlling ventilation fans with specific duty cycles&#8203;:contentReference[oaicite:3]{index=3}.

## Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/your-username/advanced-microprocessors-lab.git
   ```
2. Open the project in your preferred IDE (e.g., Atmel Studio or VS Code with PlatformIO).
3. Compile and upload the code to the AVR microcontroller.

## Usage

- **Lab 1 (Traffic Light System)**:
    - Simulate the traffic light system by pressing the pedestrian button to switch between lights.

- **Lab 2 (Room Navigation)**:
    - Use the system to navigate a simulated robot in a room with obstacles detected via sensors.

- **Lab 3 (Fan Control)**:
    - Control the fan using the buttons to toggle between different speeds.

- **Lab 4 (Moisture Sensor)**:
    - Monitor moisture levels and trigger watering or ventilation based on soil moisture thresholds.

