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

#ifndef _MESH24_ROUTE_MGR_H
#define _MESH24_ROUTE_MGR_H

#include <Mesh24Types.h>

struct Mesh24Route {
  mesh24_addr_t from;
  mesh24_addr_t to;
  byte percent;
};

class Mesh24RouteMgr {
  public:
    Mesh24RouteMgr() {
      count = 0;
    }

    mesh24_addr_t findBestRoute(mesh24_addr_t from, mesh24_addr_t to) const {
      int bestPercent = -1;
      mesh24_addr_t bestRoute = to;
      for (int index = 0; index < count; ++index) {
        if (from == routes[index].from) {
          if (to == routes[index].to) {
            if (bestPercent < routes[index].percent) {
              bestPercent = routes[index].percent;
              bestRoute = to;
            }
          } else {
            int percent = getPercent(routes[index].to, to);
            int combinedPercent = ((int) routes[index].percent * (int) percent + 50) / 100;
            if (bestPercent < combinedPercent) {
              bestPercent = combinedPercent;
              bestRoute = routes[index].to;
            }
          }
        }
      }
#ifdef MESH24_DEBUG_ROUTE_MGR
      Serial.print(millis());
      Serial.print(" Mesh24RouteMgr.findBestRoute(): from=");
      Serial.print(from);
      Serial.print(" to=");
      Serial.print(to);
      Serial.print(" bestRoute=");
      Serial.print(bestRoute);
      Serial.println();
#endif
      return bestRoute;
    }

    void set(mesh24_addr_t from, mesh24_addr_t to, byte percent) {
#ifdef MESH24_DEBUG_ROUTE_MGR
      Serial.print(millis());
      Serial.print(" Mesh24RouteMgr.set(): from=");
      Serial.print(from);
      Serial.print(" to=");
      Serial.print(to);
      Serial.print(" percent=");
      Serial.print(percent);
      Serial.println();
#endif
      int index = findIndex(from, to);
      if (index == -1) {
        index = create(from, to, 50);
      }
      set(index, percent);
    }

    byte update(mesh24_addr_t from, mesh24_addr_t to, bool ok) {
#ifdef MESH24_DEBUG_ROUTE_MGR
      Serial.print(millis());
      Serial.print(" Mesh24RouteMgr.update(): from=");
      Serial.print(from);
      Serial.print(" to=");
      Serial.print(to);
      Serial.print(" ok=");
      Serial.print(ok ? 1 : 0);
      Serial.println();
#endif
      int index = findIndex(from, to);
      if (index == -1) {
        index = create(from, to, 50);
      }
      return update(index, ok);
    }

    void printRoutes(Stream& stream) {
      for (int index = 0; index < count; ++index) {
        stream.print(" ");
        stream.print(routes[index].from);
        stream.print(",");
        stream.print(routes[index].to);
        stream.print("->");
        stream.print(routes[index].percent);
      }
    }

  private:
    byte getPercent(mesh24_addr_t from, mesh24_addr_t to) const {
      int index = findIndex(from, to);
      return index == -1 ? 0 : routes[index].percent;
    }

    int findIndex(mesh24_addr_t from, mesh24_addr_t to) const {
      for (int index = 0; index < count; ++index) {
        if ((from == routes[index].from) && (to == routes[index].to)) {
          return index;
        }
      }
      return -1;
    }

    int create(mesh24_addr_t from, mesh24_addr_t to, byte percent) {
      int index = count == 32 ? 31 : count;
      if (count < 32) {
        ++count;
      }
      routes[index].from = from;
      routes[index].to = to;
      routes[index].percent = percent;
      return index;
    }

    byte update(int index, bool ok) {
      int percent = routes[index].percent;
      if (ok) {
        percent += max((105 - percent) / 10, 1);
      } else {
        percent -= max((percent + 5) / 10, 1);
      }
      percent = max(min(percent, 100), 0);
      set(index, percent);
      return percent;
    }

    void set(int index, byte percent) {
      Mesh24Route route = routes[index];
      route.percent = percent;
      while (index) {
        routes[index] = routes[index - 1];
        --index;
      }
      routes[0] = route;
#ifdef MESH24_DEBUG_ROUTE_MGR
      Serial.print(millis());
      Serial.print(" Mesh24RouteMgr.update():");
      printRoutes(Serial);
      Serial.println();
#endif
    }

    int count;
    Mesh24Route routes[32];
};

#endif
