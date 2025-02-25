// Copyright (c) 2015-present, Qihoo, Inc.  All rights reserved.
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree. An additional grant
// of patent rights can be found in the PATENTS file in the same directory.

#include "include/pika_data_distribution.h"

const std::string kTagBegin = "{";
const std::string kTagEnd = "}";

void HashModulo::Init() {}

uint32_t HashModulo::Distribute(const std::string& str, uint32_t partition_num) {
  return std::hash<std::string>()(str) % partition_num;
}

void Crc32::Init() { Crc32TableInit(IEEE_POLY); }

void Crc32::Crc32TableInit(uint32_t poly) {
  int i, j;
  for (i = 0; i < 256; i++) {
    uint32_t crc = i;
    for (j = 0; j < 8; j++) {
      if (crc & 1) {
        crc = (crc >> 1) ^ poly;
      } else {
        crc = (crc >> 1);
      }
    }
    crc32tab[i] = crc;
  }
}

uint32_t Crc32::Distribute(const std::string& str, uint32_t partition_num) {
  std::string key = GetHashkey(str);
  uint32_t crc = Crc32Update(0, key.data(), (int)key.size());
  assert(partition_num != 0);
  return crc % partition_num;
}

uint32_t Crc32::Crc32Update(uint32_t crc, const char* buf, int len) {
  int i;
  crc = ~crc;
  for (i = 0; i < len; i++) {
    crc = crc32tab[(uint8_t)((char)crc ^ buf[i])] ^ (crc >> 8);
  }
  return ~crc;
}

std::string GetHashkey(const std::string& key) {
  auto beg = key.find_first_of(kTagBegin);
  if (beg == std::string::npos) {
    return key;
  }
  auto end = key.find_first_of(kTagEnd, beg + 1);
  if (end == std::string::npos) {
    return key;
  } else {
    return key.substr(beg + 1, end - beg - 1);
  }
}
