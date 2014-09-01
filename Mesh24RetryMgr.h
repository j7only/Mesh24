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

#ifndef _MESH24_RETRY_MGR_H
#define _MESH24_RETRY_MGR_H

class Mesh24RetryMgr {
  public:
    Mesh24RetryMgr() {
      reset();
    }

    bool isTimeToRetry() {
      return time + delay <= millis();
    }

    void reset() {
      time = 0;
      delay = 0;
    }

    void inc() {
      time = millis();
      delay = max(min(delay * 2UL, 60000UL), 1000UL);
    }

  private:
    unsigned long time;
    unsigned long delay;
};

#endif
