#pragma once

#include <map>
#include <stack>
#include <thread>

#include <string>

#include "queue.h"
#include "timestamp.h"
#include "extendedtimestamp.h"
#include "extendedtimestats.h"

class Measurement {

public:
	Measurement();
	~Measurement();

	void time_before(const char *id2iperf_contextName);

	void time_after(int statementCount);

	void report();

protected:
	void consumerThread();

private:
	int mMeasurementsCount;
	bool mConsumerRunning;
	std::thread mConsumerThread;
	Queue<Timestamp> mQueue;
	TimestampFactory mFactory;

	std::map<const char *, ExtendedTimeStats> mStats;

	std::stack<ExtendedTimestamp> mStack;

	

};
