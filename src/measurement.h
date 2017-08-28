#pragma once

#include <map>
#include <stack>
#include <thread>
#include <iostream>
#include <string>

#include "queue.h"
#include "timestamp.h"
#include "timestats.h"

class Measurement {

public:
	Measurement();
	~Measurement();

	void time_before(const char *id2iperf_contextName);

	void time_after();

	void report();

protected:
	void consumerThread();

private:

	bool mConsumerRunning;
	std::thread mConsumerThread;
	Queue<Timestamp> mQueue;
	TimestampFactory mFactory;

	std::map<std::string, TimeStats> mStats;

	std::stack<Timestamp> mStack;

};