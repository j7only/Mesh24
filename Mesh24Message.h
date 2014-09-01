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

#ifndef _MESH24_MESSAGE_H
#define _MESH24_MESSAGE_H

#include <Mesh24Types.h>

#define MESH24_MESSAGE_SIZE 16
#define MESH24_PAYLOAD_SIZE 12

#define MESH24_MSG_CREATE_SESSION_REQ 0xFF
#define MESH24_MSG_CREATE_SESSION_RESP 0xFE
#define MESH24_MSG_ROUTE_UPDATE 0xFD
#define MESH24_MSG_HEARTBEAT 0xFC

#define MESH24_MSG_DIN_READ_REQ 0xEF
#define MESH24_MSG_DIN_READ_RSP 0xEE
#define MESH24_MSG_DIN_CHANGED_MSG 0xED
#define MESH24_MSG_DIN_CHANGED_ACK 0xEC

#define MESH24_MSG_AIN_READ_MSG 0xE1

class Mesh24Message {
  public:
    Mesh24Message(): length(0), position(0) {
    }
    
    Mesh24Message(mesh24_addr_t to, byte type): length(0), position(0) {
      setTo(to);
      setType(type);
    }
    
    Mesh24Message(mesh24_addr_t from, mesh24_addr_t to, byte type): length(0), position(0) {
      setFrom(from);
      setTo(to);
      setType(type);
    }
    
    mesh24_addr_t getFrom() const {
      return buffer[0];
    }
    
    void setFrom(mesh24_addr_t from) {
      buffer[0] = from;
    }

    mesh24_addr_t getTo() const {
      return buffer[1];
    }
    
    void setTo(mesh24_addr_t from) {
      buffer[1] = from;
    }

    byte getType() const {
      return buffer[2];
    }
    
    void setType(byte type) {
      buffer[2] = type;
    }
    
    template <class T>
    void readPayload(T& data) {
      readPayload(position, data);
      position += sizeof(data);
    }
    
    template <class T>
    void readPayload(int offset, T& data) const {
      if (offset < 0) {
        offset = MESH24_PAYLOAD_SIZE + offset;
      }
      data = *(T*)(buffer + 3 + offset);
    }
    
    template <class T>
    void writePayload(const T& data) {
      writePayload(length, data);
      length += sizeof(data);
    }

    template <class T>
    void writePayload(int offset, const T& data) {
      if (offset < 0) {
        offset = MESH24_PAYLOAD_SIZE + offset;
      }
      *(T*)(buffer + 3 + offset) = data;
    }
    
    byte* getBuffer() {
      return buffer;
    }

    const byte* getBuffer() const {
      return buffer;
    }
    
  private:
    byte length;
    byte position;
    byte buffer[MESH24_MESSAGE_SIZE];
};

#endif
