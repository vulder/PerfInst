#ifdef __linux__

#include "timestamp.h"


TimestampFactory::TimestampFactory() :
	mHandle(pthread_self()) {

}

void TimestampFactory::getPlatformCurrent(PlatformTimestamp &ts) {

	

}

TimeStats PlatformTimestamp::operator-(const PlatformTimestamp &start) const {
	
	TimeStats diff;

	return diff;
}

#endif