#pragma once


#include <atomic>
#include <array>

/**
 * A single-producer, single-consumer queue.
 */

template <class T, size_t bufferSize = 64>
class Queue {
public:

	Queue() :
		mReadIndex(0), mWriteIndex(0)
	{
	}
	
	bool try_push(const T &begin, const T &end)
	{
		if (full())
		{
			return false;
		}else
		{
			mData[mWriteIndex] = { begin, end };
			increment(mWriteIndex);
			return true;
		}
	}

	void push(const T &begin, const T &end) {
		if (full()) {
			throw 0;
		}
		else {
			mData[mWriteIndex] = { begin, end };
			increment(mWriteIndex);
		}
	}

	bool consume(std::pair<T, T> &data)
	{
		if (!empty()) {
			data = mData[mReadIndex];
			increment(mReadIndex);			
			return true;
		}
		else
			return false;
	}

	void wait(bool waitForEmpty = false, bool yield = false)
	{
		if (waitForEmpty) {
			while (!empty())
			{
				if (yield)
					std::this_thread::yield();
			}
		}else
		{
			while (full())
			{
				if (yield)
					std::this_thread::yield();
			}
		}
	}

	bool empty() const {
		return mWriteIndex == mReadIndex;
	}

	bool full() const
	{
		return next(mWriteIndex) == mReadIndex;
	}

protected:
	void increment(std::atomic<int> &index) {
		index = next(index);
	}

	static int next(int index){
		return (index + 1) % bufferSize;
	}

private:
	
	std::atomic<int> mReadIndex, mWriteIndex;

	std::array<std::pair<T, T>, bufferSize> mData;

};
