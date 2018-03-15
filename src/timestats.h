#pragma once

#include "clock.h"


struct TimeStats {
	int mStatementCount;
	perf_clock::duration mDuration;
	std::chrono::microseconds mUserTime;
	std::chrono::microseconds mKernelTime;

	TimeStats() :
		mStatementCount(0), mDuration(0), mUserTime(0), mKernelTime(0) {}

	void operator += (const TimeStats &other) {
		mDuration += other.mDuration;
		mUserTime += other.mUserTime;
		mKernelTime += other.mKernelTime;
		mStatementCount += other.mStatementCount;
	}

	TimeStats operator - (const TimeStats &other) const
	{
		TimeStats result;
		result.mDuration = mDuration - other.mDuration;
		result.mUserTime = mUserTime - other.mUserTime;
		result.mKernelTime = mKernelTime - other.mKernelTime;
		result.mStatementCount = mStatementCount - other.mStatementCount;
		return result;
	}
};