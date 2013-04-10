cmake_minimum_required(VERSION 2.6)
project(Kast_Vis)

#Version number:
set (Kasterborous_Visualizer_VERSION_MAJOR 0)
set (Kasterborous_Visualizer_VERSION_MINOR 1)
#Configure a header file to pass CMake stuff to the source code:
configure_file (
  ${PROJECT_SOURCE_DIR}/kast_vis.h.in
  ${PROJECT_BINARY_DIR}/kast_vis.h
)
#------------------------------------------------
#Add all executables:
#------------------------------------------------
#General files:
set (GENERAL	
	src/main.cpp	
)
#------------------------------------------------
#Set subdirectories:
#------------------------------------------------
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_executable(kast_vis ${GENERAL})
#------------------------------------------------
#Handle LUA implementation:
#------------------------------------------------
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})
find_package(Lua52 REQUIRED)

if(LUA52_FOUND)
	include_directories(${LUA_INCLUDE_DIR})
	target_link_libraries(kast_vis ${LUA_LIBRARY})
endif(LUA52_FOUND)

#------------------------------------------------
#Handle FLTK includes
#------------------------------------------------
#find_package(FLTK REQUIRED NO_MODULE)

find_package(FLTK REQUIRED NO_MODULE) 
include(${FLTK_USE_FILE})

target_link_libraries(kast_vis fltk)
#------------------------------------------------
#Set compiler flags
#------------------------------------------------
set(CMAKE_CXX_FLAGS "-pthread -std=c++11")


