#include "timestamp.h"

struct ExtendedTimestamp
{
	ExtendedTimestamp(Timestamp &&time, const TimeStats &overheadSnapshot, bool outermost) :
		mTimestamp(std::forward<Timestamp>(time)), 
		mOverhead(overheadSnapshot),
		mOutermost(outermost) {}

	Timestamp mTimestamp;
	TimeStats mOverhead;
	bool mOutermost;
};