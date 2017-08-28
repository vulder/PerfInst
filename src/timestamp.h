#pragma once

#include <type_traits>
#include <thread>
#include "timestats.h"

#ifdef _WIN32
struct PlatformTimestamp{

};
#endif

struct Timestamp {
	const char *mContext;
	bool mBefore;
	perf_clock::time_point mTimepoint;
	PlatformTimestamp mPlatformTimestamp;

	TimeStats operator-(const Timestamp &start) {
		TimeStats diff;
		diff.mDuration = mTimepoint - start.mTimepoint;
		return diff;
	}
};

class TimestampFactory {
public:
	TimestampFactory();
	
	Timestamp getCurrentBefore(const char * context) {
		Timestamp ts;
		ts.mContext = context;
		ts.mBefore = true;
		getCurrent(ts);
		return ts;
	}
	Timestamp getCurrentAfter() {
		Timestamp ts;
		ts.mContext = nullptr;
		ts.mBefore = false;
		getCurrent(ts);
		return ts;
	}

private:
	void getCurrent(Timestamp &ts) {
		ts.mTimepoint = perf_clock::now();
		getPlatformCurrent(ts.mPlatformTimestamp);
	}
	void getPlatformCurrent(PlatformTimestamp &ts);

private:
	decltype(std::declval<std::thread>().native_handle()) mHandle;
};

