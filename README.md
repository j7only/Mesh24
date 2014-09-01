# Mesh24 - Arduino Mesh Network Library for nRF24L01/+ Radio

## Design Goals

TBA

## Features

- **Encrypted and secure communication**  
Not just all messages are protected using [AES256](http://en.wikipedia.org/wiki/Advanced_Encryption_Standard) encryption, Mesh24 can secure communication against [replay attacks](http://en.wikipedia.org/wiki/Replay_attack)

- **Routing and path management**  
Each node in the network acts as a router forwarding message between other nodes which are out of range from each other.  Algorithm implemented automatically keeps track the success rate of sending and chooses the best path based on that.

- **Built-in status message sent every 5 minutes**  
Nodes in the network will send a status message in every five minutes.  (The frequency is configurable and this feature can be even turned off, if unwanted.)  The status message contains
  - Uptime of the node in seconds (will __not__ roll over after about 50 days as millis() call does),
  - Voltage using atmega's internal voltage reading, and
  - Temperature using atmega's internal temperature reading.

## Dependencies

- **RF24** - Arduino driver for nRF24L01  
See [master](https://github.com/maniacbug/RF24) and the [fork](https://github.com/j7only/RF24) tested with

- **AESLib** - Arduino Library for AES Encryption  
See [master](https://github.com/DavyLandman/AESLib) and the [fork](https://github.com/j7only/AESLib) tested with

- **Low-Power** (needed for some examples only) - Low Power Library for Arduino  
See [master](https://github.com/rocketscream/Low-Power) and the [fork](https://github.com/j7only/Low-Power) tested with


## Installation

- Download the files in this repository and the dependent libraries listed above (using either clone or the download button)

- Copy the Mesh24, RF24, AESLib, and LowPower folders into arduino/libraries folder

- Restart Arduino IDE and check out the examples


## Examples

TBA

## Documentation

TBA
