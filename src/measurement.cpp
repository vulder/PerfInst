#include "measurement.h"
#include <set>
#include "extendedtimestats.h"
#include <iostream>
#ifdef __linux__
#include <pthread.h>
#endif
#include <unordered_map>
#include <thread>
#include <queue>


Measurement::Measurement() :
	mMeasurementsCount(0),
	mConsumerRunning(true),
	mConsumerThread(&Measurement::consumerThread, this),
	mTimestampBuffer(&mBuffer1),
	mTimestampSwap(&mBuffer2)
{

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

	*mTimestampSwap = mFactory.getCurrentBefore("BASE");
	mTimestampAfter = *mTimestampSwap;
}

Measurement::~Measurement() {
	
	mConsumerRunning = false;
	if (mConsumerThread.joinable())
		mConsumerThread.join();
}

void Measurement::time_before(const char *id2iperf_contextName) {

	*mTimestampBuffer = mFactory.getCurrentBefore(id2iperf_contextName);
	std::swap(mTimestampBuffer, mTimestampSwap);
	mQueue.wait();
	mQueue.push(*mTimestampBuffer, mTimestampAfter);
	mTimestampAfter = mFactory.getCurrentBefore(id2iperf_contextName);
}

void Measurement::time_after(int statementCount) {
	*mTimestampBuffer = mFactory.getCurrentAfter(statementCount);
	std::swap(mTimestampBuffer, mTimestampSwap);
	mQueue.wait();
	mQueue.push(*mTimestampBuffer, mTimestampAfter);
	mTimestampAfter = mFactory.getCurrentAfter(statementCount);
}

void Measurement::report() {
	*mTimestampBuffer = mFactory.getCurrentAfter(0);
	mQueue.wait();
	mQueue.push(*mTimestampSwap, mTimestampAfter);
	mQueue.wait();
	mQueue.push(*mTimestampBuffer, *mTimestampBuffer);

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
	}

	std::cout <<
		"Total time: " <<
		std::chrono::duration_cast<std::chrono::nanoseconds>(mStats["BASE"].mStats.mDuration).count() / 1000000.0f <<
		" ms (overhead: " <<
		std::chrono::duration_cast<std::chrono::nanoseconds>(mStats["BASE"].mOverhead.mDuration).count() / 1000000.0f <<
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
					TimeStats overhead = start.mOverhead;
					overhead += (timestamps.second - timestamps.first);
					stats.mOverhead += overhead;
					if (!start.mOutermost)
					{
						stats.mStats.mStatementCount += timestamps.first.mStatementCount;
					}else
					{
						stats.mStats += timestamps.second - start.mTimestamp;
						auto it = context.find(start.mTimestamp.mContext);
						stats.mMeasurements += it->second;
						context.erase(it);
					}
					++mMeasurementsCount;
					mStack.pop();
					if (!mStack.empty()){
						mStack.top().mOverhead += overhead;
					}
				}
			}
			//std::this_thread::yield();
		}
	}
