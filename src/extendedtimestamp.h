#include "timestamp.h"

struct ExtendedTimestamp
{
	ExtendedTimestamp(Timestamp &&t, const Timestamp &begin_t, bool outermost) : 
		mTimestamp(std::forward<Timestamp>(t)), 
		mOverhead(mTimestamp - begin_t),
		mOutermost(outermost) {}

	Timestamp mTimestamp;
	TimeStats mOverhead;
	bool mOutermost;
};