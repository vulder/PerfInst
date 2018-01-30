Project(PerfInst)

add_library(PerfInst STATIC include/perfinst.h src/measurement.cpp src/measurement.h src/perfinst.cpp src/queue.h src/timestamp.h src/timestamp_win.cpp src/timestats.h)

#add_executable(test test/main.c)

#target_link_libraries(test PerfInst)