#pragma once


#include <atomic>
#include <array>

/**
 * A single-producer, single-consumer queue.
 */

template <class T, size_t bufferSize = 32>
class Queue {
public:

	Queue() :
		mReadIndex(0), mWriteIndex(0)
	{
		mNotMissedPackages.clear();
	}
	

	void push(const T &data) {
		if (next(mWriteIndex) == mReadIndex) {
			mNotMissedPackages.clear();
		}
		else {
			mData[mWriteIndex] = data;
			increment(mWriteIndex);
		}
	}

	bool consume(T &data, bool &missedPackages)
	{
		if (!empty()) {
			data = mData[mReadIndex];
			missedPackages = mNotMissedPackages.test_and_set();
			increment(mReadIndex);
			return true;
		}
		else
			return false;
	}

	bool empty() const {
		return mWriteIndex == mReadIndex;
	}

protected:
	void increment(std::atomic<int> &index) {
		index = next(index);
	}

	int next(int index) {
		return (index + 1) % bufferSize;
	}

private:
	
	std::atomic<int> mReadIndex, mWriteIndex;

	std::atomic_flag mNotMissedPackages;

	std::array<T, bufferSize> mData;

};