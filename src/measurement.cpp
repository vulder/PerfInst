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
	using namespace std::chrono_literals;
	perf_clock::duration total_overhead = 0ns;

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

	for (const std::pair<const std::string, ExtendedTimeStats> &featureStats : mStats) {
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

		total_overhead += featureStats.second.mOverhead.mDuration;
	}

	std::cout <<
		"Total time: " <<
		std::chrono::duration_cast<std::chrono::nanoseconds>(mStats["BASE"].mStats.mDuration - total_overhead).count() / 1000000.0f <<
		" ms (overhead: " <<
		std::chrono::duration_cast<std::chrono::nanoseconds>(total_overhead).count() / 1000000.0f <<
		")"
	<< std::endl;
	std::cout << "-- Hercules Performance End --" << std::endl;
}


	void Measurement::consumerThread() {

		std::pair<Timestamp, Timestamp> timestamps;
		std::unordered_map<const char *, int> context;
		
		std::string prefix;
		std::stack<int> prefixIndices;
		
		TimeStats overheadAccumulator;

		while (mConsumerRunning || !mQueue.empty()) {
			if (mQueue.consume(timestamps)) {
				if (timestamps.first.mBefore) {
					const auto &pib = context.insert({timestamps.first.mContext,0});

					if (pib.second){
						prefixIndices.push(prefix.size());
						if (!prefix.empty()){
							if (prefix == "BASE"){
								prefix.clear();
							}else{
								prefix += '#';
							}
						}
						prefix += timestamps.first.mContext;
					}
					
					mStack.emplace(std::move(timestamps.first), overheadAccumulator, pib.second);
					overheadAccumulator += timestamps.second - timestamps.first;
					
					++(pib.first->second);
					
				}
				else {
					
					
					ExtendedTimestamp &start = mStack.top();
					ExtendedTimeStats &stats = mStats[prefix];

					overheadAccumulator += timestamps.second - timestamps.first;

					if (!start.mOutermost)
					{
						stats.mStats.mStatementCount += timestamps.first.mStatementCount;
					}else
					{						
						TimeStats overhead = overheadAccumulator - start.mOverhead;
						stats.mOverhead += overhead;
						stats.mStats += timestamps.second - start.mTimestamp - overhead;
						auto it = context.find(start.mTimestamp.mContext);
						stats.mMeasurements += it->second;
						context.erase(it);

						//Line necessary for HERCULES to work correctly. Comment out for more accurate results.
						overheadAccumulator -= overhead;
						
						prefix = prefix.substr(0, prefixIndices.top());
						prefixIndices.pop();

					}
					
					++mMeasurementsCount;
					mStack.pop();
				}
			}
			//std::this_thread::yield();
		}
	}
