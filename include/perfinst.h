#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void id2iperf_time_start();
void id2iperf_time_end();

void id2iperf_time_before(char *id2iperf_contextName);
void id2iperf_time_after(int statementCount);
//void id2iperf_terminate();

#ifdef __cplusplus
}
#endif