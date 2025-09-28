# CMakeList.txt : CMake project for VfWcodecs, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("RLEcodec")

# Add source to this project's executable.
add_executable (RLEvfwencode "RLEvfwencode.c" "DriverProc.c" "codec.c" "RLEalgorithm.c" "RLEcodec.h")
add_executable (RLEvfwdecode "RLEvfwdecode.c" "DriverProc.c" "codec.c" "RLEalgorithm.c" "RLEcodec.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET RLEvfwencode PROPERTY CXX_STANDARD 20)
  set_property(TARGET RLEvfwdecode PROPERTY CXX_STANDARD 20)
endif()


# TODO: Add tests and install targets if needed.
