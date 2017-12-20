#include "timestamp.h"

#include <Windows.h>

#ifdef _WIN32

TimestampFactory::TimestampFactory() :
	mHandle(GetCurrentThread()) {

}

void TimestampFactory::getPlatformCurrent(PlatformTimestamp &ts) {

	if (GetThreadTimes(mHandle, &ts.mCreationTime, &ts.mExitTime, &ts.mKernelTime, &ts.mUserTime) == FALSE) {
		throw 0;
	}

}

TimeStats PlatformTimestamp::operator-(const PlatformTimestamp &start) {
	ULARGE_INTEGER startUser;
	startUser.LowPart = start.mUserTime.dwLowDateTime;
	startUser.HighPart = start.mUserTime.dwHighDateTime;

	ULARGE_INTEGER user;
	user.LowPart = mUserTime.dwLowDateTime;
	user.HighPart = mUserTime.dwHighDateTime;

	ULARGE_INTEGER startKernel;
	startKernel.LowPart = start.mKernelTime.dwLowDateTime;
	startKernel.HighPart = start.mKernelTime.dwHighDateTime;

	ULARGE_INTEGER kernel;
	kernel.LowPart = mKernelTime.dwLowDateTime;
	kernel.HighPart = mKernelTime.dwHighDateTime;

	TimeStats diff;
	diff.mUserTime = std::chrono::milliseconds((user.QuadPart - startUser.QuadPart) / 10);
	diff.mKernelTime = std::chrono::milliseconds((kernel.QuadPart - startKernel.QuadPart) / 10);

	return diff;
}

#endif