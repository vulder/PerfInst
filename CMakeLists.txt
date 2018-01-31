SET(CMAKE_BUILD_TYPE Release)
SET(CMAKE_CXX_FLAGS_RELEASE "-O2")

Project(PerfInst)

set (CMAKE_CXX_STANDARD 14)

find_package (Threads)



add_library(PerfInst SHARED include/perfinst.h src/measurement.cpp src/measurement.h src/perfinst.cpp src/queue.h src/timestamp.h src/timestamp_win.cpp src/timestamp_linux.cpp src/timestats.h)

target_link_libraries (PerfInst ${CMAKE_THREAD_LIBS_INIT})

#add_executable(test test/main.c)

#target_link_libraries(test PerfInst)