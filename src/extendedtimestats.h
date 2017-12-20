#pragma once

#include "timestats.h"

struct ExtendedTimeStats
{
	ExtendedTimeStats() : mMeasurements(0) {}

	TimeStats mStats;
	int mMeasurements;
};