# Smart Sensor Network
### Distributed Embedded-Inspired Telemetry System (C++)

A C++ system that simulates a network of embedded sensor devices communicating with a central gateway using UDP telemetry and TCP-based monitoring.

The project demonstrates systems programming concepts such as networking, concurrency, and embedded software design patterns.

---

## Overview

The system simulates multiple sensor devices that:

- Generate periodic telemetry data
- Maintain internal state using finite state machines
- Send data via UDP
- Report heartbeat status

A central Gateway:

- Receives telemetry from all sensors
- Tracks device health
- Detects offline devices
- Provides a TCP monitoring interface

---

## Architecture

- **Sensor Nodes** → UDP telemetry + heartbeat
- **Gateway Server** → data aggregation + monitoring
- **Monitor Client** → TCP-based system status viewer

---

## Key Concepts

- UDP & TCP socket programming
- Multithreading & synchronization
- Finite State Machines (FSM)
- Watchdog timers
- Heartbeat monitoring
- Binary protocol design

---

## Tech Stack

- C++17/20
- Linux
- POSIX Sockets
- Multithreading
- CMake

---

## Why This Project?

This project was built to explore how embedded and IoT systems communicate in real-world environments, focusing on low-level networking, concurrency, and system reliability.

---

## Project Status

This project is currently in early development. Core components are being implemented step by step starting from the Sensor Node design.

---

## Future Improvements

- Packet loss simulation
- CRC validation
- Prometheus metrics
- MQTT support