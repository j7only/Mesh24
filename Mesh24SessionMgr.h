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

#ifndef _MESH24_SESSION_MGR_H
#define _MESH24_SESSION_MGR_H

#include <Mesh24Types.h>

struct Mesh24Session {
  unsigned long createdAt;
  mesh24_addr_t to;
  unsigned long sessionId;
};

class Mesh24SessionMgr {
  public:
    Mesh24SessionMgr() {
      for (int index = 0; index < 8; ++index) {
        sessions[index].to = 0;
      }
    }
    
    unsigned long create(mesh24_addr_t to) {
      unsigned long sessionId = random(0x7FFFFFFFL);
      add(to, sessionId);
      return sessionId;
    }
    
    void add(mesh24_addr_t to, unsigned long sessionId) {
      int index = findSlot(to);
      sessions[index].createdAt = millis();
      sessions[index].to = to;
      sessions[index].sessionId = sessionId;
#ifdef MESH24_DEBUG_SESSION_MGR
      Serial.print(millis());
      Serial.print(" Mesh24SessionMgr.add(): index=");
      Serial.print(index);
      Serial.print(" to=");
      Serial.print(to);
      Serial.print(" sessionId=");
      Serial.print(sessionId);
      Serial.println();
#endif
    }
    
    bool isValid(mesh24_addr_t to, unsigned long sessionId) const {
#ifdef MESH24_DEBUG_SESSION_MGR
      Serial.print(millis());
      Serial.print(" Mesh24SessionMgr.isValid(): to=");
      Serial.print(to);
      Serial.print(" sessionId=");
      Serial.print(sessionId);
#endif
      unsigned long now = millis();
      for (int index = 0; index < 8; ++index) {
        if ((to == sessions[index].to) && (now - sessions[index].createdAt < 10000)) {
#ifdef MESH24_DEBUG_SESSION_MGR
          Serial.print(" valid=true");
          Serial.println();
#endif
          return true;
        }
      }
#ifdef MESH24_DEBUG_SESSION_MGR
      Serial.print(" valid=false");
      Serial.println();
#endif
      return false;
    }
    
  private:
    int findSlot(mesh24_addr_t to) const {
      unsigned long now = millis();
      int oldestIndex = 0;
      unsigned long oldestAge = now - sessions[0].createdAt;
      for (int index = 0; index < 8; ++index) {
        if (sessions[index].to == to) {
          return index;
        }
        unsigned long age = now - sessions[index].createdAt;
        if (oldestAge < age) {
          oldestIndex = index;
          oldestAge = age;
        }
      }
      return oldestIndex;
    }
    
    Mesh24Session sessions[8];
};

#endif
