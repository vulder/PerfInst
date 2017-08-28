
#include "../include/perfinst.h"
#include "measurement.h"

static Measurement sMeasurement;


void id2iperf_time_after()
{
	sMeasurement.time_after();
}

void id2iperf_time_before(char * id2iperf_contextName)
{
	sMeasurement.time_before(id2iperf_contextName);
}