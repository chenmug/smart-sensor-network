# Smart Sensor Network

### Distributed Embedded-Inspired Telemetry System (C++)

A C++ project that simulates a network of embedded-inspired sensor devices communicating with a central gateway using UDP telemetry and a TCP monitoring interface.

The project explores systems programming concepts including socket programming, multithreading, binary protocols, and embedded software architecture.

---

## Overview

The current implementation includes:

* Simulated sensor nodes generating telemetry
* Binary serialization of sensor messages
* UDP-based telemetry transmission
* Central Gateway maintaining the latest state of each sensor
* TCP server exposing a simple monitoring interface
* Unit tests for core components using GoogleTest


---

## Current Architecture

```text
+-------------+
| Sensor Node |
+-------------+
      |
      | UDP
      v
+-------------+
|   Gateway   |
+-------------+
      ^
      | TCP
      |
+-------------+
| TCP Client  |
+-------------+
```

---

## Current Features

* Sensor simulation
* UDP socket communication
* TCP monitoring server
* Binary telemetry protocol
* Multithreaded execution
* Gateway state management
* GoogleTest unit tests

---

## Planned Features

* Heartbeat monitoring
* Finite State Machines (FSM)
* Offline sensor detection
* Watchdog timers
* Multiple sensor types (Temperature, Pressure, Battery)
* Packet loss simulation
* CRC validation
* Prometheus metrics
* MQTT support

---

## Tech Stack

* C++17
* Linux
* POSIX Sockets
* TCP/IP
* UDP
* Multithreading
* CMake
* GoogleTest

---

## Why This Project?

The goal of this project is to gain hands-on experience building software commonly found in embedded and IoT systems, focusing on networking, concurrency, and modular software architecture.

Rather than targeting specific hardware, the project simulates how embedded devices exchange telemetry with a central monitoring service.

---

## Project Status

**Work in Progress**

The networking infrastructure, telemetry pipeline, and monitoring interface are implemented. Additional embedded-oriented features such as heartbeat monitoring and fault detection are currently under development.
