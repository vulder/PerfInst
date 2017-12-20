#include "measurement.h"
#include <set>
#include "extendedtimestats.h"


Measurement::Measurement() :
	mConsumerRunning(true),
	mMeasurementsCount(0),
	mConsumerThread(&Measurement::consumerThread, this)
{
	mQueue.push(mFactory.getCurrentBefore("BASE"));
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

	for (const std::pair<const std::string, ExtendedTimeStats> &featureStats : mStats) {
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
		std::map<std::string, int> context;
		while (mConsumerRunning || !mQueue.empty()) {
			if (mQueue.consume(timestamp, missedPackages)) {
				std::cout << missedPackages << std::endl;
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
			std::this_thread::yield();
		}
	}
