#ifndef STATION_UTILS_H_
#define STATION_UTILS_H_

#include <Arduino.h>

namespace STATION_Utils {

void deleteNotHeard();
void updateLastHeard(String station);
bool wasHeard(String station);
void checkBuffer();
void updatePacketBuffer(String packet);
bool hasHeardSomeone();

}

#endif