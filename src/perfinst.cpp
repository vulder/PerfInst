
#include "../include/perfinst.h"
#include "measurement.h"

static Measurement *sMeasurement;



void id2iperf_time_after(int statementCount)
{
	sMeasurement->time_after(statementCount);
}

void id2iperf_time_before(char * id2iperf_contextName)
{
	sMeasurement->time_before(id2iperf_contextName);
}

void id2iperf_time_start()
{
	sMeasurement = new Measurement;
}

void id2iperf_time_end()
{
	sMeasurement->report();
	delete sMeasurement;
}
