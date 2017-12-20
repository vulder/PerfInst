#include "../include/perfinst.h"

#include "windows.h"

#include "stdio.h"

int main() {

	id2iperf_time_start();

	id2iperf_time_before("test");

	id2iperf_time_before("test");
	Sleep(300);
	id2iperf_time_after(1);

	Sleep(700);

	id2iperf_time_before("test2");

	Sleep(500);

	id2iperf_time_after(1);

	Sleep(200);

	id2iperf_time_after(2);

	id2iperf_time_end();

	printf("Press Any Key to Continue\n");
	getchar();

	return 0;
}