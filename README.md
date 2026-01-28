# Agricultural Field Simulation (C++)

## Project Overview

This project is a simulated agricultural monitoring system developed in C++.  
It was originally created as part of an academic assignment and later refactored and extended as a **standalone demonstrative project**.

The goal of the project is to design and implement a modular and concurrent system capable of simulating an agricultural field with heterogeneous physical conditions, autonomous monitoring vehicles, sensors, and a central control unit for data coordination and analysis.

The simulation focuses on:
- physical field modeling
- sensor-based data acquisition
- multi-threaded coordination of autonomous agents
- data aggregation and analysis

---

## Main Features

- **Agricultural field simulation**  
  Customizable field dimensions with heterogeneous physical properties (soil type, temperature, moisture, crops).

- **Crop-aware monitoring**  
  Cells with crops are prioritized during analysis, enabling targeted data evaluation.

- **Autonomous vehicles and sensors**  
  Vehicles navigate the field, collect data via sensors, and send it to a control center.

- **Centralized control and data analysis**  
  A control center coordinates vehicle movements and processes collected data.

- **Concurrent programming**  
  Multi-threaded architecture using mutexes and condition variables to ensure safe and efficient execution.

---

## Simulated Agricultural Field

The field is represented as a `length × width` matrix, where each cell is an instance of the `Soil` class.

Each soil cell includes:
- **Soil type**: clay, sand, loam, or silt  
  (affects soil temperature and analysis thresholds)
- **Air temperature and humidity**
- **Soil moisture**
- **Crop presence flag**

Soil temperature is automatically computed based on soil type and environmental conditions.

The user can:
- modify soil properties in specific areas
- change field dimensions
- inspect field data via getter and print functions

Physical parameters cannot be accessed directly by the user and must be measured through sensors, mimicking real-world constraints.

---

## Vehicles

Autonomous vehicles are represented by the `Vehicle` class and are used to collect data across the field.

Each vehicle is characterized by:
- position within the field
- movement speed (cells/sec)
- battery level

Battery consumption is simulated during movement and data acquisition.  
When a minimum threshold is reached, the vehicle must return to base for recharging before resuming operations.

### Key vehicle functions

- `setPosition`
- `moveToTarget`
- `readDataFromCurrentCell`
- `readAndSendData`

Battery management and vehicle operations are protected via mutexes to prevent race conditions and deadlocks.

---

## Control Center

The `ControlCenter` class is responsible for:
- issuing movement commands
- collecting sensor data
- buffering and analyzing measurements
- identifying critical field conditions and their coordinates

The control center operates concurrently with vehicles and uses condition variables to synchronize data availability and analysis.

### Core functions

- `sendMovementCommandToVehicle`
- `commandDataRead`
- `appendData`
- `analyzeData`

---

## Sensors

Sensors are modeled by the `Sensor` class and can measure:
- soil temperature
- soil moisture
- air humidity

To increase realism, sensor readings include a configurable random noise component that simulates measurement uncertainty.

---

## Concurrent Programming Design

The system relies heavily on concurrency to simulate realistic parallel operations.

### Key concepts

- **Threads**  
  Vehicles and the control center run on separate threads.

- **Mutexes**  
  Used to protect shared resources such as:
  - vehicle position maps
  - data buffers

- **Condition Variables**  
  Enable efficient synchronization between data producers (vehicles) and consumers (control center).

This design improves:
- performance
- responsiveness
- scalability when adding vehicles or sensors

---

## Current Limitations

- Environmental conditions are static and hardcoded.
- The simulation represents a snapshot in time rather than a fully dynamic evolution.

Despite this, the model remains suitable for analyzing field conditions at a given instant and for demonstrating concurrent system design.


