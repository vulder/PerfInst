#pragma once

#include "clock.h"


struct TimeStats {
	perf_clock::duration mDuration;

	void operator += (const TimeStats &other) {
		mDuration += other.mDuration;
	}
};