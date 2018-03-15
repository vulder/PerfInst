#include "timestamp.h"

struct ExtendedTimestamp
{
	ExtendedTimestamp(Timestamp &&begin_t, const Timestamp &end_t, bool outermost) :
		mTimestamp(std::forward<Timestamp>(begin_t)), 
		mOverhead(end_t - mTimestamp),
		mOutermost(outermost) {}

	Timestamp mTimestamp;
	TimeStats mOverhead;
	bool mOutermost;
};