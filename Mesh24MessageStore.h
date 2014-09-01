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

#ifndef _MESH24_MESSAGE_STORE_H
#define _MESH24_MESSAGE_STORE_H

#include <Mesh24Types.h>

struct Mesh24MessageStoreElement {
  unsigned long createdAt;
  mesh24_addr_t to;
  unsigned long id;
  byte buffer[MESH24_MESSAGE_SIZE];
};

class Mesh24MessageStore {
  public:
    Mesh24MessageStore() {
      for (int index = 0; index < 4; ++index) {
        elements[index].to = 0;
      }
    }
    
    unsigned long add(const Mesh24Message& message) {
      int index = findSlot();
      elements[index].createdAt = millis();
      elements[index].to = message.getTo();
      elements[index].id = random(0x7FFFFFFFL);
      memcpy(elements[index].buffer, message.getBuffer(), MESH24_MESSAGE_SIZE);
#ifdef MESH24_DEBUG_MESSAGE_STORE
      Serial.print(millis());
      Serial.print(" Mesh24MessageStore.add(): to=");
      Serial.print(elements[index].to);
      Serial.print(" id=");
      Serial.print(elements[index].id);
      Serial.print(" index=");
      Serial.print(index);
      Serial.println();
#endif
      return elements[index].id;
    }
    
    bool remove(Mesh24Message& message, mesh24_addr_t to, unsigned long id) {
#ifdef MESH24_DEBUG_MESSAGE_STORE
      Serial.print(millis());
      Serial.print(" Mesh24MessageStore.remove(): to=");
      Serial.print(to);
      Serial.print(" id=");
      Serial.print(id);
      Serial.print(" index=");
#endif
      for (int index = 0; index < 4; ++index) {
        if ((elements[index].to == to) && (elements[index].id == id)) {
          memcpy(message.getBuffer(), elements[index].buffer, MESH24_MESSAGE_SIZE);
          elements[index].to = 0;
#ifdef MESH24_DEBUG_MESSAGE_STORE
      Serial.print(index);
      Serial.println();
#endif
          return true;
        }
      }
#ifdef MESH24_DEBUG_MESSAGE_STORE
      Serial.println();
#endif
      return false;
    }
    
  private:
    int findSlot() {
      unsigned long now = millis();
      int oldestIndex = 0;
      unsigned long oldestAge = now - elements[0].createdAt;
      for (int index = 0; index < 4; ++index) {
        if (elements[index].to == 0) {
          return index;
        }
        unsigned long age = now - elements[index].createdAt;
        if (age > oldestAge) {
          oldestIndex = index;
          oldestAge = age;
        }
      }
      return oldestIndex;
    }
    
    Mesh24MessageStoreElement elements[4];
};

#endif
