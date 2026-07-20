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
* Binary serialization and deserialization of telemetry and heartbeat messages
* UDP-based telemetry and heartbeat communication
* Central Gateway maintaining sensor state
* Heartbeat-based sensor health monitoring
* Automatic offline detection using heartbeat timeout
* TCP monitoring interface for inspecting sensor information
* Thread-safe logging system
* Unit tests using GoogleTest

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
        SensorNode
            |
    +----------------+
    |                |
Telemetry        Heartbeat
    |                |
    UDP              UDP
    |                |
    +-------+--------+
            |
            v
         Gateway
            |
     +------+------+
     |             |
 Watchdog       TCP Server
     |             |
 Health          Monitor
 Tracking          |
                TCP Client


All components
      |
      v
+-------------+
|   Logger    |
+-------------+
```

---

## Monitoring Interface

The system exposes a TCP monitoring interface that allows querying the current state of connected sensors.

Example:

```text
$ nc 127.0.0.1 8080


help

Available commands:
-------------------
list              - Show all sensors
get <id>          - Show sensor details
health            - Show sensor health status
stats             - Show system statistics
help              - Show available commands



list
=== SENSOR LIST ===

ID    TYPE           STATE       HEALTH      LAST HB
-----------------------------------------------------
1     Motion         ACTIVE      ONLINE      2 sec
2     Motion         ACTIVE      ONLINE      2 sec
3     Temperature    ACTIVE      OFFLINE     20 sec
4     Temperature    ACTIVE      ONLINE      2 sec
5     Battery        ACTIVE      ONLINE      2 sec
6     Pressure       ACTIVE      ONLINE      2 sec
7     Pressure       ACTIVE      OFFLINE     20 sec


get 2
=== SENSOR INFORMATION ===

Packet Header
--------------
messageType : TELEMETRY
sensorId    : 2
timestamp   : 1784213736100

Telemetry Payload
-----------------
sensorType  : Motion
state       : ACTIVE
value       : 0.879716

Heartbeat Status
----------------
health         : ONLINE
last heartbeat : 2 sec ago



health

=== HEALTH SUMMARY ===

ONLINE  : 5
OFFLINE : 2
UNKNOWN : 0

Offline sensors:
- Sensor 3 (Temperature)
- Sensor 7 (Pressure)



stats

=== SYSTEM STATISTICS ===

Total sensors      : 7
Telemetry packets  : 7
Heartbeat messages : 49

```

---

## Current Features

#### Sensor simulation framework
* Object-oriented sensor hierarchy
* Multiple sensor implementations:
  * Motion sensor
  * Temperature sensor
  * Pressure sensor
  * Battery sensor

#### Communication
* UDP socket communication
* TCP monitoring server
* Binary packet protocol supporting telemetry and heartbeat messages

#### Gateway & Monitoring

* Central sensor registry
* Latest telemetry storage
* Heartbeat timestamp tracking
* Sensor health tracking
* Watchdog-based offline detection
* TCP monitoring commands:
  * help
  * list
  * get <sensor_id>
  * health
  * stats
* Thread-safe access to shared sensor data

#### Software Design
* Synchronization using mutexes and condition variables
* Interface-based design for improved testability
* GoogleTest unit tests for core components

---

## Planned Features

* Packet loss simulation
* CRC validation
* Prometheus metrics
* MQTT support

---

## Demo

Example monitoring session:

```text
help
list
get 3
health
stats
```
---

## Design Highlights

* Layered architecture separating networking, gateway, monitoring, and sensor logic
* Binary protocol shared between sensors and the Gateway
* Thread-safe shared state protected using mutexes
* Interface-based design enabling isolated unit testing with fake implementations
* Watchdog thread responsible for offline sensor detection

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

Implemented:

* Multi-threaded sensor simulation
* UDP telemetry and heartbeat communication
* Binary serialization protocol
* Gateway state management
* Heartbeat-based watchdog
* Offline sensor detection
* TCP monitoring interface
* Command-based monitoring (help, list, get, health, stats)

Planned:

* Packet loss simulation
* CRC validation
* Prometheus metrics
* MQTT support

Future improvements focus on protocol robustness and additional embedded-system features.