// Uncomment all or some of the following lines to turn on debug
// These must be before the "#include <Mesh24>"
//#define MESH24_DEBUG
//#define MESH24_DEBUG_MESSAGE_STORE
//#define MESH24_DEBUG_ROUTE_MGR
//#define MESH24_DEBUG_SESSION_MGR

#include <AESLib.h>
#include <Mesh24.h>
#include <RF24.h>
#include <SPI.h>

// nRF24L01/+ must be connected to digital pins 9, 10, 11, 12, 13
// Out of the above 9 and 10 can be reconfigured if needed
// Digital pins 11, 12 and 13 are for SPI and cannot be changed
Mesh24 mesh24(9, 10);

void setup() {
  Serial.begin(57600);
  
  // All nodes in the mesh network must have the same channel
  // It must be between 0 and 127
  // Uncomment the follow line to change the default which is 75
  // Use value less then 80 in US to comply with FCC (at least that is my understanding)
//  mesh24.setChannel(60);

  // Must be unique for all nodes
  // Heartbeats from all nodes are sent to address 1
  mesh24.setAddr(1);
  
  // Initialize radio
  mesh24.begin();
}

void loop() {
  Mesh24Message message;

  // Read new message, if any
  if (mesh24.read(message)) {
    // Print message content in hex
    Serial.print(millis());
    Serial.print(" msg=");
    Mesh24Utils.printAsHex(Serial, message.getBuffer(), MESH24_MESSAGE_SIZE);
    Serial.println();
    
    // If heartbeat
    if (message.getType() == MESH24_MSG_HEARTBEAT) {
      // Parse parameters (uptime, temperature, and voltage)
      unsigned long uptime;
      unsigned temperature;
      unsigned voltage;
      message.readPayload(uptime);
      message.readPayload(temperature);
      message.readPayload(voltage);
      
      // Print parameters
      Serial.print("device=");
      Serial.print(message.getFrom());

      // To convert raw reading of temperature to F or C,
      // a one time calibration is needed (see docs for details)
      Serial.print(" temperature=");
      Serial.print(temperature);
      
      // Raw reading of voltage can be converted to mV as below
      // Accuracy is +/- 10%.  That means board running on 5V
      // can report values from 4500mV to 5500mV after conversion.
      Serial.print(" voltage=");
      Serial.print(1126400 / voltage);
      
      // Uptime is in seconds and will not roll over after about
      // 50 days
      Serial.print("mV uptime=");
      Serial.print(uptime);
      Serial.println("s");
    }
  }
  
  // Give the mcu a little break
  delay(1);
}

