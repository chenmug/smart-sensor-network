# Smart Sensor Network

### Distributed Embedded-Inspired Telemetry System (C++)

A C++ project that simulates a distributed network of embedded-inspired sensor devices communicating with a central Gateway using UDP telemetry and a TCP monitoring interface.

The project explores systems programming concepts including socket programming, multithreading, synchronization, binary protocols, serialization, and modular software architecture.

---

## Overview

The system simulates a distributed sensor network where multiple sensor nodes generate telemetry data and communicate with a central Gateway.

The current implementation includes:

* Simulated sensor nodes generating telemetry data
* Multiple sensor types: Motion, Temperature, Pressure, and Battery
* Binary serialization and deserialization of telemetry and heartbeat messages
* Explicit Big Endian wire format
* UDP-based telemetry and heartbeat communication
* Central Gateway maintaining sensor state and latest telemetry
* Heartbeat-based sensor health monitoring
* Automatic offline detection using heartbeat timeout
* TCP monitoring interface for inspecting sensor information
* Thread-safe access to shared Gateway state
* Unit tests using GoogleTest

---

## Why Both UDP and TCP?

The project intentionally separates telemetry traffic from monitoring traffic, following a communication pattern commonly used in embedded and distributed systems.

### UDP Telemetry

Sensor readings are transmitted over UDP because telemetry traffic can favor low latency and low protocol overhead.

In continuous telemetry streams, losing an occasional measurement may be acceptable because newer measurements can replace older ones.

UDP is also used for heartbeat messages.

### TCP Monitoring

Monitoring commands use TCP because administrative communication requires reliable and ordered delivery.

Examples include:

* Querying sensor status
* Requesting the latest sensor reading
* Inspecting sensor health
* Viewing system statistics

---

## Current Architecture

```text
                         Sensor Nodes
                              |
                    +---------+---------+
                    |                   |
               Telemetry            Heartbeat
                    |                   |
                   UDP                 UDP
                    |                   |
                    +---------+---------+
                              |
                              v
                           Gateway
                              |
                    +---------+---------+
                    |                   |
                 Watchdog           TCP Server
                    |                   |
             Health Tracking       Monitoring
                                        |
                                        v
                                   TCP Client


                    All Components
                           |
                           v
                       +-------+
                       | Logger|
                       +-------+
```

The Gateway acts as the central communication and state-management component.

Each sensor node periodically generates telemetry and heartbeat messages. The Gateway receives the messages, updates the latest sensor information, and tracks sensor health based on heartbeat activity.

---

## Binary Protocol

The Sensor Network uses a custom binary protocol for communication between sensor nodes and the Gateway.

The protocol defines explicit field sizes and byte ordering instead of relying on C++ struct memory layout.

All multi-byte integer fields are serialized in **Big Endian (network byte order)**.


### Protocol Enum Values

| Message Type | Value |
|--------------|------:|
| `TELEMETRY`  | 1     |
| `HEARTBEAT`  | 2     |

| Sensor Type  | Value |
|--------------|------:|
| `Motion`     | 1     |
| `Temperature`| 2     |
| `Battery`    | 3     |
| `Pressure`   | 4     |

| Sensor State | Value |
|--------------|------:|
| `ACTIVE`     | 1     |
| `WARNING`    | 2     |
| `ERROR`      | 3     |


### Common Packet Header

Both telemetry and heartbeat messages contain the following header:

| Field        |    Size | Encoding               |
| ------------ | ------: | ---------------------- |
| Message Type |  1 byte | `uint8_t`              |
| Sensor ID    | 4 bytes | `uint32_t`, Big Endian |
| Timestamp    | 8 bytes | `uint64_t`, Big Endian |

The common header is therefore **13 bytes**.

### Telemetry Message

A telemetry packet consists of the common header followed by the telemetry payload:

| Offset | Size | Field        | Encoding                           |
| -----: | ---: | ------------ | ---------------------------------- |
|      0 |    1 | Message Type | `uint8_t`                          |
|      1 |    4 | Sensor ID    | `uint32_t`, Big Endian             |
|      5 |    8 | Timestamp    | `uint64_t`, Big Endian             |
|     13 |    1 | Sensor Type  | `uint8_t`                          |
|     14 |    1 | Sensor State | `uint8_t`                          |
|     15 |    8 | Sensor Value | `double`, serialized in Big Endian |

**Total size: 23 bytes**

### Heartbeat Message

A heartbeat contains only the common header:

| Offset | Size | Field        | Encoding               |
| -----: | ---: | ------------ | ---------------------- |
|      0 |    1 | Message Type | `uint8_t`              |
|      1 |    4 | Sensor ID    | `uint32_t`, Big Endian |
|      5 |    8 | Timestamp    | `uint64_t`, Big Endian |

**Total size: 13 bytes**


### Floating-Point Serialization

Sensor measurements are represented as C++ `double` values.

The serializer preserves the raw 64-bit representation of the `double` and serializes those bits in Big Endian order. The deserializer reconstructs the original `double` from the received bit representation.

The protocol does not serialize C++ structs directly. Each field is serialized explicitly to avoid dependencies on compiler-specific padding, alignment, or host memory layout.

### Protocol Validation

The deserializer validates that enough bytes are available before reading each field and reports buffer-underflow errors for incomplete packets.

---

## Monitoring Interface

The system exposes a TCP monitoring interface that allows querying the current state of connected sensors.

Example using `netcat`:

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
-------------
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
Telemetry packets  : 77
Heartbeat messages : 32
Online sensors     : 5
Offline sensors    : 2
```

---

## Current Features

### Sensor Simulation

* Object-oriented sensor hierarchy
* Multiple sensor implementations:

  * Motion sensor
  * Temperature sensor
  * Pressure sensor
  * Battery sensor
* Periodic telemetry generation
* Periodic heartbeat generation

### Communication

* UDP socket communication
* TCP monitoring server
* Custom binary packet protocol
* Telemetry and heartbeat message types
* Explicit Big Endian serialization
* Binary serialization/deserialization
* Packet validation during deserialization

### Gateway & Monitoring

* Central sensor registry
* Latest telemetry storage
* Heartbeat timestamp tracking
* Separate sensor state and health tracking
* Watchdog-based offline detection
* TCP monitoring commands:

  * `help`
  * `list`
  * `get <sensor_id>`
  * `health`
  * `stats`
* Thread-safe access to shared sensor data

### Software Design

* Layered architecture separating sensor, networking, Gateway, and monitoring logic
* Interface-based design for improved testability
* Synchronization using mutexes and condition variables
* GoogleTest unit tests for core components
* Fake UDP sender for isolated sensor-node testing

---

## Testing

The project uses **GoogleTest** for unit testing.

Current tests cover core sensor and sensor-node functionality.

Packet serialization/deserialization tests are being added to verify:

* Telemetry round-trip serialization
* Heartbeat round-trip serialization
* Packet sizes
* Big Endian byte representation
* Enum wire values
* `double` serialization/deserialization
* Buffer-underflow handling
* Protocol boundary conditions

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

* Layered architecture separating networking, Gateway, monitoring, and sensor logic
* Binary protocol shared between sensor nodes and the Gateway
* Explicit wire format independent of C++ struct memory layout
* Big Endian serialization using bit operations
* Separate sensor state and communication health tracking
* Thread-safe shared state protected using mutexes
* Interface-based design enabling isolated unit testing
* Fake network sender used for unit tests
* Watchdog thread responsible for offline sensor detection

---

## Tech Stack

* C++17
* Linux
* POSIX Sockets
* TCP/IP
* UDP
* Multithreading
* `std::mutex`
* `std::condition_variable`
* Object-Oriented Design
* CMake
* GoogleTest

---

## Why This Project?

The goal of this project is to gain hands-on experience building software commonly found in embedded and IoT systems, with a focus on networking, concurrency, binary communication protocols, and modular software architecture.

Rather than targeting specific hardware, the project simulates how embedded-inspired sensor devices exchange telemetry with a central monitoring service.

The project was built to strengthen practical systems-programming and networking skills while applying software-design and testing principles in a multi-component C++ system.

---

## Project Status

**Work in Progress**

### Implemented

* Multi-threaded sensor simulation
* Multiple sensor types
* UDP telemetry and heartbeat communication
* Custom binary serialization protocol
* Explicit Big Endian wire format
* Gateway state management
* Heartbeat-based watchdog
* Offline sensor detection
* TCP monitoring interface
* Command-based monitoring:

  * `help`
  * `list`
  * `get`
  * `health`
  * `stats`
* Thread-safe shared Gateway state
* Unit testing infrastructure with GoogleTest


### Planned

* Packet loss simulation
* CRC validation
* Prometheus metrics
* MQTT support

Future improvements focus on protocol robustness, testing coverage, and additional embedded-system communication features.