# CMakeList.txt : CMake project for VfWcodecs, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("JXScodec")

# Add source to this project's executable.
add_executable (JXS_mono_encode "JXS_mono_encode.c" "file_io.h")
add_executable (JXSvfwencode "JXSvfwencode.c" "DriverProc.c" "codec.c" "JXSalgorithm.c" "JXScodec.h")
add_executable (JXSvfwdecode "JXSvfwdecode.c" "DriverProc.c" "codec.c" "JXSalgorithm.c" "JXScodec.h")
add_executable (JXS_encode "JXS_encode.c" "file_io.h")

target_link_libraries(JXS_mono_encode 
	debug ${CMAKE_SOURCE_DIR}/JXScodec/libjxs/jxs.lib ${CMAKE_SOURCE_DIR}/JXScodec/libjxs/jxs_utils.lib 
	optimized ${CMAKE_SOURCE_DIR}/JXScodec/libjxs/jxs.lib ${CMAKE_SOURCE_DIR}/JXScodec/libjxs/jxs_utils.lib )

target_link_libraries(JXS_encode 
	debug ${CMAKE_SOURCE_DIR}/JXScodec/libjxs/jxs.lib ${CMAKE_SOURCE_DIR}/JXScodec/libjxs/jxs_utils.lib 
	optimized ${CMAKE_SOURCE_DIR}/JXScodec/libjxs/jxs.lib ${CMAKE_SOURCE_DIR}/JXScodec/libjxs/jxs_utils.lib )

target_link_libraries(JXSvfwencode 
	debug ${CMAKE_SOURCE_DIR}/JXScodec/libjxs/jxs.lib ${CMAKE_SOURCE_DIR}/JXScodec/libjxs/jxs_utils.lib 
	optimized ${CMAKE_SOURCE_DIR}/JXScodec/libjxs/jxs.lib ${CMAKE_SOURCE_DIR}/JXScodec/libjxs/jxs_utils.lib )

target_link_libraries(JXSvfwdecode 
	debug ${CMAKE_SOURCE_DIR}/JXScodec/libjxs/jxs.lib ${CMAKE_SOURCE_DIR}/JXScodec/libjxs/jxs_utils.lib 
	optimized ${CMAKE_SOURCE_DIR}/JXScodec/libjxs/jxs.lib ${CMAKE_SOURCE_DIR}/JXScodec/libjxs/jxs_utils.lib )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET JXSvfwencode PROPERTY CXX_STANDARD 20)
  set_property(TARGET JXSvfwdecode PROPERTY CXX_STANDARD 20)
endif()


# TODO: Add tests and install targets if needed.
