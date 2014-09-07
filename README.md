# Mesh24 - Arduino Mesh Network Library for nRF24L01/+ Radio

## Table of Contents

- [Design Goals](https://github.com/j7only/Mesh24#design-goals)
- [Features](https://github.com/j7only/Mesh24#features)
- [Dependencies](https://github.com/j7only/Mesh24#dependencies)
- [Installation](https://github.com/j7only/Mesh24#installation)
- [Examples](https://github.com/j7only/Mesh24#examples)
- [Documentation](https://github.com/j7only/Mesh24#documentation)

## Design Goals

- Ease of use
- Well-suited for home and office automation
- Secure and safe communication
- Reliable
- Support battery operation

## Features

Mesh24 implements a network of nodes communicating through nRF24L01/+.  Any two of the nodes can directly communicate with each other if they are in range.  If not, another node between them acts as a message routing to forward the messages back and forth.  The network requires no configuration as each node dynamically discovers and maintains which other nodes they can communicate with directly and indirectly through another node.

- **Up to 255 nodes**  
Mesh24 allows up to 255 nodes to work together in a single network where any two can communicate with each other.

- **Zero configuration of network topology**  
Each node automatically keeps track of which nodes are in range and what is the best path to send a message to another node.

- **Encrypted and secure communication**  
Not just all messages are protected using [AES256](http://en.wikipedia.org/wiki/Advanced_Encryption_Standard) encryption, Mesh24 can secure communication against [replay attacks](http://en.wikipedia.org/wiki/Replay_attack).  See documentation for more details.

- **Can run for a year or more on a single coin battery**  
Mesh24 can power up and down the radio to conserve battery life.  Combined that with putting the processor in sleep mode, a node can run a year or more on a single coin battery, such as, CR2032, while waking up once every few minutes to send sensor readings.  See examples and documentation for more details.

- **Built-in heartbeat message sent every 5 minutes**  
By default, nodes in the network will send a heartbeat message in every five minutes.  (This can be turned off, and the frequency is configurable.)  Heartbeat can be used to detect when a node goes off-line.  The message contains
  - Uptime of the node in seconds (will __not__ roll over after about 50 days as millis() call does),
  - Voltage using atmega's internal voltage reading, and
  - Temperature using atmega's internal temperature reading.

- **16-byte messages containing 9-byte payload**  
Mesh24's messages are 16-byte long (to make it encryption friendly).  9 out of the 16 bytes carries the payload including the message type (a single byte) whose values of 128 and above are reserved for internal use.  The rest of the 7 bytes contains the message header, crc, and bytes to randomize or secure communication.

## Dependencies

- **RF24** - Arduino driver for nRF24L01/+  
Testing is done against [this fork](https://github.com/j7only/RF24)  

- **AESLib** - Arduino Library for AES Encryption  
Testing is done against [this fork](https://github.com/j7only/AESLib)  

- **Low-Power** - Low Power Library for Arduino  
`Optional` - Needed for some of the examples only  
Testing is done against [this fork](https://github.com/j7only/Low-Power)  

## Installation

- Download the files in this repository and the dependent libraries listed above (using either clone or the download button)

- Copy the Mesh24, RF24, AESLib, and LowPower folders under arduino/libraries folder

- Restart Arduino IDE and check out the examples


## Examples

TBA

## Documentation

TBA - Please see comments in examples for now
