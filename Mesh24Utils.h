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

#ifndef _MESH24_UTILS_H
#define _MESH24_UTILS_H

#include <Arduino.h>
#include <util/crc16.h>

class Mesh24UtilsClass {
  public:
    byte crc(const void* buffer, unsigned length) {
      byte result = 0;
      const byte* b = (const byte*) buffer;
      while (length--) {
        result = _crc_ibutton_update(result, *b++);
      }
      return result;
    }

    void printAsHex(Stream& stream, const byte* data, int length) {
      for (int index = 0; index < length; ++index) {
        printAsHex(stream, data[index]);
      }
    }

    void printAsHex(Stream& stream, byte data) {
      Serial.print(printAsHexDigit(data >> 4));
      Serial.print(printAsHexDigit(data & 0xf));
    }

    char* printAsHex(char* buf, const byte* data, int length) {
      for (int index = 0; index < length; ++index) {
        printAsHex(buf + index * 2, data[index]);
      }
      buf[length * 2] = 0;
      return buf;
    }

    void printAsHex(char* buf, byte data) {
      buf[0] = printAsHexDigit(data >> 4);
      buf[1] = printAsHexDigit(data & 0xf);
    }

    char printAsHexDigit(byte data) {
      data = data & 0xf;
      return data < 10 ? '0' + data : 'A' + data - 10;
    }

    bool scanAsHex(byte* data, const char* buf, int length) {
      int index = 0;
      while (length >= 2) {
        if (!scanAsHex(data[index++], buf)) {
          return false;
        }
        length -= 2;
        buf += 2;
      }
      return length == 0;
    }

    bool scanAsHex(byte& data, const char* buf) {
      byte digit1, digit2;
      if (!scanAsHexDigit(digit1, buf[0])) {
        return false;
      }
      if (!scanAsHexDigit(digit2, buf[1])) {
        return false;
      }
      data = (digit1 << 4) | digit2;
      return true;
    }

    bool scanAsHexDigit(byte& data, char c) {
      if (('0' <= c) && (c <= '9')) {
        data = c - '0';
        return true;
      }
      if (('A' <= c) && (c <= 'F')) {
        data = c - 'A' + 10;
        return true;
      }
      if (('a' <= c) && (c <= 'f')) {
        data = c - 'a' + 10;
        return true;
      }
      return false;
    }

};

Mesh24UtilsClass Mesh24Utils;

#endif
