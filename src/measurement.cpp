#include "measurement.h"


Measurement::Measurement() :
	mConsumerRunning(true),
	mConsumerThread(&Measurement::consumerThread, this)
{
}

Measurement::~Measurement() {

	mConsumerRunning = false;
	mConsumerThread.join();

	report();

	printf("Press Any Key to Continue\n");
	getchar();
}

void Measurement::time_before(const char *id2iperf_contextName) {
	mQueue.push(mFactory.getCurrentBefore(id2iperf_contextName));
}

void Measurement::time_after() {
	mQueue.push(mFactory.getCurrentAfter());
}

void Measurement::report() {
	for (const std::pair<const std::string, TimeStats> &featureStats : mStats) {
		std::cout << featureStats.first << ":" << std::endl;
		std::cout << featureStats.second.mDuration.count() << std::endl;
	}
}


	void Measurement::consumerThread() {
		bool missedPackages;
		Timestamp timestamp;
		while (mConsumerRunning || !mQueue.empty()) {
			if (mQueue.consume(timestamp, missedPackages)) {
				if (timestamp.mBefore) {
					mStack.emplace(std::move(timestamp));
				}
				else {
					Timestamp &start = mStack.top();
					mStats[start.mContext] += (timestamp - start);
					mStack.pop();
				}
			}
		}
	}
