#include "timestamp.h"

#include <Windows.h>

#ifdef _WIN32

TimestampFactory::TimestampFactory() :
mHandle(GetCurrentThread()) {

}

void TimestampFactory::getPlatformCurrent(PlatformTimestamp &ts) {
}

#endif