#include "../include/perfinst.h"

#include "windows.h"

#include "stdio.h"

int main() {

	id2iperf_time_before("test");

	Sleep(1000);

	id2iperf_time_before("test2");

	Sleep(500);

	id2iperf_time_after();

	Sleep(200);

	id2iperf_time_after();

	return 0;
}