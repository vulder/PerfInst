#include "measurement.h"
#include <set>
#include "extendedtimestats.h"
#include <iostream>
#ifdef __linux__
#include <pthread.h>
#endif
#include <unordered_map>
#include <thread>


Measurement::Measurement() :
	mMeasurementsCount(0),
	mConsumerRunning(true),
	mConsumerThread(&Measurement::consumerThread, this)
{
	Timestamp ts = mFactory.getCurrentBefore("BASE");
	mQueue.push(ts, ts);

#ifdef __linux__
	cpu_set_t cpuset;

	CPU_ZERO(&cpuset);

	for (int i = 0; i <= 4; ++i)
	{
		CPU_SET(i, &cpuset);
		CPU_SET(i + 10, &cpuset);
	}

	if (pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) != 0)
	{
		std::cout << "Could not set thread affinity. Wrong machine? Aborting!" << std::endl;
		abort();
	}

	CPU_ZERO(&cpuset);

	for (int i = 5; i <= 9; ++i)
	{
		CPU_SET(i, &cpuset);
		CPU_SET(i + 10, &cpuset);
	}

	
	if (pthread_setaffinity_np(mConsumerThread.native_handle(), sizeof(cpu_set_t), &cpuset) != 0)
	{
		std::cout << "Could not set thread affinity. Wrong machine? Aborting!" << std::endl;
		abort();
	}
#endif

}

Measurement::~Measurement() {
	
	mConsumerRunning = false;
	if (mConsumerThread.joinable())
		mConsumerThread.join();
}

void Measurement::time_before(const char *id2iperf_contextName) {

	Timestamp ts = mFactory.getCurrentBefore(id2iperf_contextName);
	if (!mQueue.try_push(ts, ts))
	{
		mQueue.wait();
		Timestamp ts2 = mFactory.getCurrentBefore(id2iperf_contextName);
		mQueue.push(ts2, ts);
	}
}

void Measurement::time_after(int statementCount) {
	Timestamp ts = mFactory.getCurrentAfter(statementCount);
	if (!mQueue.try_push(ts, ts))
	{
		mQueue.wait();
		Timestamp ts2 = mFactory.getCurrentAfter(statementCount);
		mQueue.push(ts2, ts);
	}
}

void Measurement::report() {
	perf_clock::duration fullOverhead;

	Timestamp ts = mFactory.getCurrentAfter(0);
	mQueue.push(ts, ts);
	mConsumerRunning = false;
	mConsumerThread.join();

	std::cout << "Remaining stack size: " << mStack.size() << std::endl;

	std::cout << "-- Hercules Performance --" << std::endl;
	std::cout << "Hashmap size: " << mStats.size() << std::endl;
	std::cout << "Measurement counter: " << mMeasurementsCount << std::endl;

	for (const std::pair<const char * const, ExtendedTimeStats> &featureStats : mStats) {
		std::cout << 
			featureStats.first << 
			" -> " << 
			(featureStats.second.mStats.mDuration.count() / 1000000.0f) << 
			" ms, " <<
			(featureStats.second.mOverhead.mDuration.count() / 1000000.0f) <<
			" ms (measurements: " << 
			featureStats.second.mMeasurements << 
			"; statements: " << 
			featureStats.second.mStats.mStatementCount << 
			")" << 
			std::endl;
		fullOverhead += featureStats.second.mOverhead.mDuration;
	}

	std::cout << 
		"Total time: " << 
		std::chrono::duration_cast<std::chrono::milliseconds>(mStats["BASE"].mStats.mDuration).count() << 
		" ms (overhead: " <<
		std::chrono::duration_cast<std::chrono::milliseconds>(fullOverhead).count() <<
		")"
	<< std::endl;
	std::cout << "-- Hercules Performance End --" << std::endl;
}


	void Measurement::consumerThread() {

		std::pair<Timestamp, Timestamp> timestamps;
		std::unordered_map<const char *, int> context;
		while (mConsumerRunning || !mQueue.empty()) {
			if (mQueue.consume(timestamps)) {
				if (timestamps.first.mBefore) {
					const auto &pib = context.insert({timestamps.first.mContext,0});
					mStack.emplace(std::move(timestamps.first), timestamps.second, pib.second);
					++(pib.first->second);
				}
				else {
					ExtendedTimestamp &start = mStack.top();
					ExtendedTimeStats &stats = mStats[start.mTimestamp.mContext];
					stats.mOverhead += start.mOverhead;
					stats.mOverhead += (timestamps.first - timestamps.second);

					if (!start.mOutermost)
					{
						stats.mStats.mStatementCount += timestamps.first.mStatementCount;
					}else
					{
						stats.mStats += timestamps.first - start.mTimestamp;
						auto it = context.find(start.mTimestamp.mContext);
						stats.mMeasurements += it->second;
						context.erase(it);
						++mMeasurementsCount;
					
					}
					mStack.pop();
				}
			}
			//std::this_thread::yield();
		}
	}
