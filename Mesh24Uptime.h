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

#ifndef _MESH24_UPTIME_H
#define _MESH24_UPTIME_H

#include <Arduino.h>

class Mesh24UptimeClass {
  public:
    Mesh24UptimeClass(): lastMillis(0), overflowCounter(0) {
    }

    unsigned long getSeconds() {
      unsigned long now = millis();
      if (now < lastMillis) {
        ++overflowCounter;
      }
      lastMillis = now;
      uint64_t overflowMultiplier = 0xfffffffful;
      return (lastMillis + overflowCounter * overflowMultiplier) / 1000;
    }

    unsigned long getLastMillis() {
      return lastMillis;
    }

    void setLastMillis(unsigned long millis) {
      lastMillis = millis;
    }

  private:
    unsigned long lastMillis;
    byte overflowCounter;
};

Mesh24UptimeClass Mesh24Uptime;

#endif
