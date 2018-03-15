#include "../include/perfinst.h"

#include "stdio.h"

#include <unistd.h>


int main() {

	id2iperf_time_start();

	id2iperf_time_before("test");

	id2iperf_time_before("test");
	usleep(300);
	id2iperf_time_after(1);

	usleep(700);

	id2iperf_time_before("test2");

	usleep(500);

	id2iperf_time_after(1);

	usleep(200);

	id2iperf_time_after(2);

	id2iperf_time_end();

	printf("Press Any Key to Continue\n");
	getchar();

	return 0;
}