# CMakeList.txt : CMake project for VfWcodecs, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("MRLEcodecs")

# Add source to this project's executable.
add_executable (MRLEencode "MRLencode.cpp" "MRLE.h")
add_executable (MRLEdecode "MRLdecode.cpp" "MRLE.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET MRLEencode PROPERTY CXX_STANDARD 20)
  set_property(TARGET MRLEdecode PROPERTY CXX_STANDARD 20)
endif()


# TODO: Add tests and install targets if needed.
