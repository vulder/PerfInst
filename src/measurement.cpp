#include "measurement.h"
#include <set>
#include "extendedtimestats.h"
#include <iostream>
#include <pthread.h>
#include <unordered_map>


Measurement::Measurement() :
	mMeasurementsCount(0),
	mConsumerRunning(true),
	mConsumerThread(&Measurement::consumerThread, this)
{
	mQueue.push(mFactory.getCurrentBefore("BASE"));

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

}

Measurement::~Measurement() {
	
	mConsumerRunning = false;
	if (mConsumerThread.joinable())
		mConsumerThread.join();
}

void Measurement::time_before(const char *id2iperf_contextName) {
	mQueue.push(mFactory.getCurrentBefore(id2iperf_contextName));
}

void Measurement::time_after(int statementCount) {
	mQueue.push(mFactory.getCurrentAfter(statementCount));
}

void Measurement::report() {
	mQueue.push(mFactory.getCurrentAfter(0));
	mConsumerRunning = false;
	mConsumerThread.join();

	std::cout << "Remaining stack size: " << mStack.size() << std::endl;

	std::cout << "-- Hercules Performance --" << std::endl;
	std::cout << "Hashmap size: " << mStats.size() << std::endl;
	std::cout << "Measurement counter: " << mMeasurementsCount << std::endl;

	for (const std::pair<const char *, ExtendedTimeStats> &featureStats : mStats) {
		std::cout << 
			featureStats.first << 
			" -> " << 
			std::chrono::duration_cast<std::chrono::milliseconds>(featureStats.second.mStats.mDuration).count() << 
			", 0 ms (measurements: " << 
			featureStats.second.mMeasurements << 
			"; statements: " << 
			featureStats.second.mStats.mStatementCount << 
			")" << 
			std::endl;
	}

	std::cout << "Total time: " << std::chrono::duration_cast<std::chrono::milliseconds>(mStats["BASE"].mStats.mDuration).count() << " ms (overhead: 0)" << std::endl;
	std::cout << "-- Hercules Performance End --" << std::endl;
}


	void Measurement::consumerThread() {
	using namespace std::chrono_literals;

		bool missedPackages;
		Timestamp timestamp;
		std::unordered_map<const char *, int> context;
		while (mConsumerRunning || !mQueue.empty()) {
			if (mQueue.consume(timestamp, missedPackages)) {
				if (missedPackages)
				{
					std::cout << "Missed a package. Aborting!" << std::endl;
					abort();
				}
				if (timestamp.mBefore) {
					const auto &pib = context.insert({timestamp.mContext,0});
					mStack.emplace(std::move(timestamp), pib.second);
					++(pib.first->second);
				}
				else {
					ExtendedTimestamp &start = mStack.top();
					ExtendedTimeStats &stats = mStats[start.mTimestamp.mContext];
					if (!start.mOutermost)
					{
						stats.mStats.mStatementCount += timestamp.mStatementCount;
					}else
					{
						stats.mStats += timestamp - start.mTimestamp;
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
