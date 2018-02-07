#pragma once

#include "timestats.h"

struct ExtendedTimeStats
{
	ExtendedTimeStats() : mMeasurements(0) {}

	TimeStats mStats;
	TimeStats mOverhead;
	int mMeasurements;
};