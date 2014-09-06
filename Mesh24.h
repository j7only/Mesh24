/*
This file is part of mesh24.
Copyright (C) 2014 Miklos Lengyel (j7only@gmail.com)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _MESH24_H
#define _MESH24_H

#include <AESLib.h>
#include <Arduino.h>
#include <Mesh24Message.h>
#include <Mesh24MessageStore.h>
#include <Mesh24RouteMgr.h>
#include <Mesh24SessionMgr.h>
#include <Mesh24Temperature.h>
#include <Mesh24Timer.h>
#include <Mesh24Types.h>
#include <Mesh24Uptime.h>
#include <Mesh24Utils.h>
#include <Mesh24Voltage.h>
#include <RF24.h>
#include <SPI.h>

class Mesh24 {
  public:
    Mesh24(byte cePin, byte csPin): radio(cePin, csPin), channel(75), subnet(0xDEADBEAFUL), addr(1), heartbeatTimer(5 * 60 * 1000UL) {
      memset(encryptionKey, 0, sizeof(encryptionKey));
    }

    void setChannel(byte channel) {
      this->channel = channel;
    }

    byte getChannel() const {
      return channel;
    }

    void setSubnet(mesh24_subnet_t subnet) {
      this->subnet = subnet;
    }

    mesh24_subnet_t getSubnet() const {
      return subnet;
    }

    void setAddr(byte addr) {
      this->addr = addr;
    }

    byte getAddr() const {
      return addr;
    }

    void setEncryptionKey(const byte* encryptionKey) {
      if (encryptionKey) {
        memcpy(this->encryptionKey, encryptionKey, sizeof(this->encryptionKey));
      }
    }

    Mesh24Timer& getHeartbeatTimer() {
      return heartbeatTimer;
    }
    
    void begin() {
      radio.begin();
      radio.setAutoAck(true);
      radio.setRetries(15, 15);
      radio.setPayloadSize(MESH24_MESSAGE_SIZE);
      radio.setChannel(channel);
      radio.setDataRate(RF24_250KBPS);
      radio.openReadingPipe(0, makeRadioAddress(0));
      radio.openReadingPipe(1, makeRadioAddress(addr));
      radio.startListening();
      radio.printDetails();
      writeHeartbeat();
    }
    
    void loop() {
      Mesh24Message message;
      read(message);
    }
  
    bool read(Mesh24Message& message) {
      return readSecured(message);
    }

    void write(Mesh24Message& message, bool secured = false) {
      return writeSecured(message, secured);
    }

    void writeHeartbeat() {
      if (addr == 1) {
        return;
      }
#ifdef MESH24_DEBUG
      Serial.print(millis());
      Serial.println(" Mesh24.writeHeartbeat()");
#endif
      Mesh24Message message(addr, 1, MESH24_MSG_HEARTBEAT);
      message.writePayload(Mesh24Uptime.getSeconds());
      message.writePayload(Mesh24Temperature.read());
      message.writePayload(Mesh24Voltage.read());
      addRandomPadding(message);
      writeRouted(message);
    }
  
    void powerDown() {
      radio.powerDown();
    }

    void powerUp() {
      radio.powerUp();
    }

  private:
    bool readSecured(Mesh24Message& message) {
      if (!readRouted(message)) {
        return false;
      }
      byte type = message.getType();
      if (type == MESH24_MSG_CREATE_SESSION_REQ) {
        processCreateSessionReq(message);
        return false;
      } else if (type == MESH24_MSG_CREATE_SESSION_RESP) {
        processCreateSessionResp(message);
        return false;
      } else {
        unsigned long sessionId;
        message.readPayload(-sizeof(sessionId), sessionId);
        return (sessionId & 0x80000000UL) || sessionMgr.isValid(message.getFrom(), sessionId);
      }
    }
    
    void processCreateSessionReq(Mesh24Message& message) {
        unsigned long sessionReqId;
        message.readPayload(sessionReqId);
        unsigned long sessionId = sessionMgr.create(message.getFrom());
        Mesh24Message response(addr, message.getFrom(), MESH24_MSG_CREATE_SESSION_RESP);
        response.writePayload(sessionReqId);
        response.writePayload(sessionId);
        addRandomPadding(response);
        writeRouted(response);
    }
  
    void processCreateSessionResp(Mesh24Message& message) {
        unsigned long sessionReqId;
        message.readPayload(sessionReqId);
        Mesh24Message response;
        if (!messageStore.remove(response, message.getFrom(), sessionReqId)) {
          return;
        }
        unsigned long sessionId;
        message.readPayload(sessionId);
        sessionMgr.add(message.getFrom(), sessionId);
        response.writePayload(-sizeof(sessionId), sessionId);
        writeRouted(response);
    }
    
    bool readRouted(Mesh24Message& message) {
      if (heartbeatTimer.isDue()) {
        writeHeartbeat();
      }
      if (!readEncrypted(message)) {
        return false;
      }
#ifdef MESH24_DEBUG
      Serial.print(millis());
      Serial.print(" Mesh24.readRouted(): msg=");
      Mesh24Utils.printAsHex(Serial, message.getBuffer(), MESH24_MESSAGE_SIZE);
      Serial.println();
#endif
      if (message.getTo() == addr) {
        if (message.getType() == MESH24_MSG_ROUTE_UPDATE) {
          processRouteUpdate(message);
          return false;
        }
        return true;
      }
      forward(message);
      return false;
    }
    
    void forward(Mesh24Message& message) {
      bool ok = writeEncrypted(message.getTo(), message, false);
      byte percent = routeMgr.update(addr, message.getTo(), ok);
      Mesh24Message routeUpdate(addr, message.getFrom(), MESH24_MSG_ROUTE_UPDATE);
      routeUpdate.writePayload(message.getTo());
      routeUpdate.writePayload(percent);
      ok = writeEncrypted(routeUpdate.getTo(), routeUpdate, false);
      routeMgr.update(addr, message.getFrom(), ok);
    }
    
    void processRouteUpdate(Mesh24Message& message) {
      routeMgr.update(addr, message.getFrom(), true);
      mesh24_addr_t to;
      message.readPayload(to);
      byte percent;
      message.readPayload(percent);
      routeMgr.set(message.getFrom(), to, percent);
    }
  
    bool readEncrypted(Mesh24Message& message) {
      if (!read(message.getBuffer())) {
        return false;
      }
      return decrypt(message.getBuffer());
    }
  
    bool read(void* buffer) {
      if (!radio.available()) {
        return false;
      }
      radio.read(buffer, MESH24_MESSAGE_SIZE);
#ifdef MESH24_DEBUG
      Serial.print(millis());
      Serial.print(" Mesh24.read(): msg=");
      Mesh24Utils.printAsHex(Serial, (byte*) buffer, MESH24_MESSAGE_SIZE);
      Serial.println();
#endif
      return true;
    }
    
    void writeSecured(Mesh24Message& message, bool secured) {
      if (message.getTo() == addr) {
        return;
      }
      message.setFrom(addr);
      if (secured) {
        unsigned long sessionReqId = messageStore.add(message);
        Mesh24Message request(addr, message.getTo(), MESH24_MSG_CREATE_SESSION_REQ);
        request.writePayload(sessionReqId);
        addRandomPadding(request);
        writeRouted(request);
      } else {
        addRandomPadding(message);
        writeRouted(message);
      }
    }

    void addRandomPadding(Mesh24Message& message) {
      unsigned long sessionId = random(0x7fffffffL) | 0x80000000UL;
      message.writePayload(-sizeof(sessionId), sessionId);
    }

    void writeRouted(const Mesh24Message& message) {
      byte to = routeMgr.findBestRoute(addr, message.getTo());
      bool ok = writeEncrypted(to, message, true);
      routeMgr.update(addr, to, ok);
    }
  
    bool writeEncrypted(mesh24_addr_t to, const Mesh24Message& message, bool broadcast = true) {
#ifdef MESH24_DEBUG
      Serial.print(millis());
      Serial.print(" Mesh24.writeEncrypted(): to=");
      Serial.print(to);
      Serial.print(" broadcast=");
      Serial.print(broadcast ? 1 : 0);
      Serial.print(" msg=");
      Mesh24Utils.printAsHex(Serial, message.getBuffer(), MESH24_MESSAGE_SIZE);
      Serial.println();
#endif
      byte buffer[MESH24_MESSAGE_SIZE];
      encrypt(buffer, message.getBuffer());
      if (write(to, buffer)) {
        return true;
      }
      if (broadcast) {
        write(0, buffer);
      }
      return false;
    }
  
    bool write(byte to, const void* buffer) {
      radio.stopListening();
      radio.openWritingPipe(makeRadioAddress(to));
      bool ok = radio.write(buffer, MESH24_MESSAGE_SIZE);
      radio.openReadingPipe(0, makeRadioAddress(0));
      radio.startListening();
#ifdef MESH24_DEBUG
      Serial.print(millis());
      Serial.print(" Mesh24.write(): to=");
      Serial.print(to);
      Serial.print(" msg=");
      Mesh24Utils.printAsHex(Serial, (const byte*) buffer, MESH24_MESSAGE_SIZE);
      Serial.print(" ok=");
      Serial.println(ok ? 1 : 0);
#endif
      return ok;
    }
    
    uint64_t makeRadioAddress(mesh24_addr_t addr) {
      return (((uint64_t) subnet) << 8) | addr;
    }
    
    void encrypt(byte* dest, const byte* src) {
      memcpy(dest, src, MESH24_MESSAGE_SIZE - 1);
      dest[MESH24_MESSAGE_SIZE - 1] = Mesh24Utils.crc(dest, MESH24_MESSAGE_SIZE - 1);
      aes256_enc_single(encryptionKey, dest);
    }
    
    bool decrypt(byte* buffer) {
      aes256_dec_single(encryptionKey, buffer);
      byte crc = Mesh24Utils.crc(buffer, MESH24_MESSAGE_SIZE - 1);
      return crc == buffer[MESH24_MESSAGE_SIZE - 1];
    }
    
    RF24 radio;
    byte channel;
    mesh24_subnet_t subnet;
    mesh24_addr_t addr;
    byte encryptionKey[32];
    Mesh24RouteMgr routeMgr;
    Mesh24SessionMgr sessionMgr;
    Mesh24MessageStore messageStore;
    Mesh24Timer heartbeatTimer;
};
  
#endif
