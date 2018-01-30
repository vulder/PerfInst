Project(PerfInst)

set (CMAKE_CXX_STANDARD 14)

add_library(PerfInst SHARED include/perfinst.h src/measurement.cpp src/measurement.h src/perfinst.cpp src/queue.h src/timestamp.h src/timestamp_win.cpp src/timestamp_linux.cpp src/timestats.h)

#add_executable(test test/main.c)

#target_link_libraries(test PerfInst)