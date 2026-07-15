# Smart Sensor Network

### Distributed Embedded-Inspired Telemetry System (C++)

A C++ project that simulates a network of embedded-inspired sensor devices communicating with a central gateway using UDP telemetry and a TCP monitoring interface.

The project explores systems programming concepts including socket programming, multithreading, synchronization, binary protocols, and embedded software architecture.

---

## Overview

The system simulates a distributed sensor network where multiple sensor nodes generate telemetry data and communicate with a central Gateway.

The current implementation includes:

* Simulated sensor nodes generating telemetry data
* Multiple sensor types (Motion, Temperature, Pressure, Battery)
* Binary serialization and deserialization of telemetry messages
* UDP-based telemetry transmission
* Central Gateway maintaining the latest state of each sensor
* TCP monitoring interface for querying sensor information
* Thread-safe logging system with timestamped events
* Unit tests for core components using GoogleTest

---

## Why Both UDP and TCP?

The project intentionally separates telemetry traffic from monitoring traffic, following a common architectural pattern used in embedded and distributed systems.

### UDP Telemetry

Sensor readings are transmitted over UDP because telemetry data favors low latency and minimal protocol overhead.

In continuous telemetry streams, losing an occasional packet is often acceptable since newer measurements quickly replace older ones.

### TCP Monitoring

Monitoring commands use TCP because administrative communication requires reliable, ordered delivery.

Examples include querying sensor status, requesting the latest readings, or inspecting the current state of the system.

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


All components
      |
      v
+-------------+
|   Logger    |
+-------------+
```

---

## Current Features

* Sensor simulation framework
* Multiple sensor implementations:
  * Motion sensor
  * Temperature sensor
  * Pressure sensor
  * Battery sensor
* UDP socket communication
* TCP monitoring server
* Binary telemetry protocol
* Gateway-based sensor state management
* Multithreaded execution
* Synchronization using mutexes and condition variables
* Thread-safe timestamped logging
* Interface-based design for improved testability
* GoogleTest unit tests for core components

---

## Planned Features

* Heartbeat monitoring
* Offline sensor detection
* Watchdog timers
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
* Synchronization primitives (mutex, condition_variable)
* Object-Oriented Design
* CMake
* GoogleTest

---

## Why This Project?

The goal of this project is to gain hands-on experience building software commonly found in embedded and IoT systems, focusing on networking, concurrency, and modular software architecture.

Rather than targeting specific hardware, the project simulates how embedded devices exchange telemetry with a central monitoring service.

---

## Project Status

**Work in Progress**

The networking infrastructure, telemetry pipeline, sensor simulation framework, and monitoring interface are implemented.

Additional embedded-oriented features such as heartbeat monitoring, offline detection, and watchdog mechanisms are currently under development.
