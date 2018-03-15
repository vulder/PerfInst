#include "../include/perfinst.h"

#include "stdio.h"

#include <chrono>
#include <thread>

int main() {

	id2iperf_time_start();

	id2iperf_time_before("test");

	id2iperf_time_before("test");
	std::this_thread::sleep_for(std::chrono::milliseconds(300));
	id2iperf_time_after(1);

	std::this_thread::sleep_for(std::chrono::milliseconds(700));

	id2iperf_time_before("test2");

	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	id2iperf_time_after(1);

	std::this_thread::sleep_for(std::chrono::milliseconds(200));

	id2iperf_time_after(2);

	id2iperf_time_end();

	printf("Press Any Key to Continue\n");
	getchar();

	return 0;
}