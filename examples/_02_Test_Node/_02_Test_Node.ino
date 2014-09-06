//#define MESH24_DEBUG
//#define MESH24_DEBUG_MESSAGE_STORE
//#define MESH24_DEBUG_ROUTE_MGR
//#define MESH24_DEBUG_SESSION_MGR

#include <AESLib.h>
#include <Mesh24.h>
#include <RF24.h>
#include <SPI.h>

Mesh24 mesh24(9, 10);
Mesh24Timer sendTimer(1000);

void setup() {
  Serial.begin(57600);
  mesh24.setChannel(60);
  mesh24.setAddr(2);
  mesh24.begin();
}

void loop() {
  Mesh24Message message;
  if (mesh24.read(message)) {
    Serial.print(millis());
    Serial.print(" msg=");
    Mesh24Utils.printAsHex(Serial, message.getBuffer(), MESH24_MESSAGE_SIZE);
    Serial.println();
  }
  if (sendTimer.isDue()) {
    mesh24.writeHeartbeat();
  }
  delay(1);
}
