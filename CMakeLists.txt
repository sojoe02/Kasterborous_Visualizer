cmake_minimum_required(VERSION 2.6)
project(Kast_Vis)

#Version number:
set (kast_vis_VERSION_MAJOR 0)
set (kast_vis_VERSION_MINOR 451)
#Configure a header file to pass CMake stuff to the source code:
configure_file (
  ${PROJECT_SOURCE_DIR}/src/kast_vis.h.in
  ${PROJECT_BINARY_DIR}/kast_vis.h
)
#------------------------------------------------
#Add all executables:
#------------------------------------------------
#General files:
set (GENERAL	
	src/main.cpp
	src/utility.h
	src/event.h
	src/ui.h
	src/ui.cpp
	src/maphandler.cpp
	src/maphandler.h
	src/physics/phys.cpp
	src/physics/phys.h
	)

set (WIDGETS
	src/widgets/colormap.cpp
	src/widgets/colormap.h
	src/widgets/intensitymap.cpp
	src/widgets/intensitymap.h
	src/widgets/iblock.cpp
	src/widgets/iblock.h
	)
#------------------------------------------------
#Set subdirectories:
#------------------------------------------------
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets)
add_executable(kast_vis ${GENERAL} ${WIDGETS})
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

find_package(FLTK REQUIRED NO_MODULE) 
include(${FLTK_USE_FILE})

find_package(PNG REQUIRED)


target_link_libraries(kast_vis fltk -lXext  -ldl -lpthread -lpng -lfltk_images)

# /usr/lib/x86_64-linux-gnu/libXext.so.6 
#target_link_libraries(kast_vis fltk)

#------------------------------------------------
#Set compiler flags
#------------------------------------------------
#set(CMAKE_CXX_FLAGS "-lpthread -ldl -std=c++11")
#set(CMAKE_C_FLAGS "-lpthread -ldl")

#+CMAKE
set(CMAKE_CXX_FLAGS "-std=c++11 -g")
#set(CMAKE_C_FLAGS "-lpthread -ldl")


