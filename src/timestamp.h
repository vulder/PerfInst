#pragma once

#include <type_traits>
#include <thread>
#include "timestats.h"

#ifdef _WIN32

#include <Windows.h>
#include <WinBase.h>

typedef HANDLE NativeHandle;

struct PlatformTimestamp{
	FILETIME mCreationTime;
	FILETIME mExitTime;
	FILETIME mKernelTime;
	FILETIME mUserTime;

	TimeStats operator-(const PlatformTimestamp &start);
};
#endif

struct Timestamp {
	const char *mContext;
	bool mBefore;
	int mStatementCount;
	perf_clock::time_point mTimepoint;
	PlatformTimestamp mPlatformTimestamp;

	TimeStats operator-(const Timestamp &start) {
		TimeStats diff = mPlatformTimestamp - start.mPlatformTimestamp;
		diff.mDuration = mTimepoint - start.mTimepoint;
		diff.mStatementCount = mStatementCount;
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
		ts.mStatementCount = 0;
		getCurrent(ts);
		return ts;
	}
	Timestamp getCurrentAfter(int statementCount) {
		Timestamp ts;
		ts.mContext = nullptr;
		ts.mBefore = false;
		ts.mStatementCount = statementCount;
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
	NativeHandle mHandle;
};

