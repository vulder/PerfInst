#include "timestamp.h"

struct ExtendedTimestamp
{
	ExtendedTimestamp(Timestamp &&t, bool outermost) : mTimestamp(std::forward<Timestamp>(t)), mOutermost(outermost) {}

	Timestamp mTimestamp;
	bool mOutermost;
};