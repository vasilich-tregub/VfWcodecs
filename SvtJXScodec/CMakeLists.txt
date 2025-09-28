# CMakeList.txt : CMake project for VfWcodecs, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("SvtJXScodec")

# Add source to this project's executable.
add_executable (SvtJXSvfwencode "SvtJXSvfwencode.c" "DriverProc.c" "codec.c" "SvtJXSalgorithm.c" "SvtJXScodec.h")
add_executable (SvtJXSvfwdecode "SvtJXSvfwdecode.c" "DriverProc.c" "codec.c" "SvtJXSalgorithm.c" "SvtJXScodec.h")
add_executable (SvtJXS_encode "SvtJXS_encode.c" "svtjxs/SvtJpegxsEnc.h")
add_executable (SvtJXSvfwXcode "SvtJXSvfwXcode.c" "DriverProc.c" "codec.c" "SvtJXSalgorithm.c" "SvtJXScodec.h")

target_link_libraries(SvtJXS_encode 
	debug ${CMAKE_SOURCE_DIR}/SvtJXScodec/svtjxs/SvtJpegxsd.lib
	optimized ${CMAKE_SOURCE_DIR}/SvtJXScodec/svtjxs/SvtJpegxsd.lib)

target_link_libraries(SvtJXSvfwencode 
	debug ${CMAKE_SOURCE_DIR}/SvtJXScodec/svtjxs/SvtJpegxsd.lib
	optimized ${CMAKE_SOURCE_DIR}/SvtJXScodec/svtjxs/SvtJpegxsd.lib)

target_link_libraries(SvtJXSvfwdecode 
	debug ${CMAKE_SOURCE_DIR}/SvtJXScodec/svtjxs/SvtJpegxsd.lib
	optimized ${CMAKE_SOURCE_DIR}/SvtJXScodec/svtjxs/SvtJpegxsd.lib)

target_link_libraries(SvtJXSvfwXcode 
	debug ${CMAKE_SOURCE_DIR}/SvtJXScodec/svtjxs/SvtJpegxsd.lib
	optimized ${CMAKE_SOURCE_DIR}/SvtJXScodec/svtjxs/SvtJpegxsd.lib)

# TODO: Add tests and install targets if needed.
