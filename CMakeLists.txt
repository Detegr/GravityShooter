cmake_minimum_required(VERSION 2.8.8)
project(GravityShooter)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/modules/")

find_package(
	Magnum REQUIRED
	Sdl2Application
	Shaders
	SceneGraph
	Primitives
	MeshTools
	DebugTools)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CORRADE_CXX_FLAGS}")
include_directories(${MAGNUM_INCLUDE_DIRS} ${MAGNUM_APPLICATION_INCLUDE_DIRS})

include_directories(BEFORE ${CMAKE_CURRENT_BINARY_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
	${CMAKE_CURRENT_BINARY_DIR}/configure.h)

add_executable(gravity src/main.cpp)
target_link_libraries(gravity
	${MAGNUM_LIBRARIES}
	${MAGNUM_MESHTOOLS_LIBRARIES}
	${MAGNUM_SHADERS_LIBRARIES}
	${MAGNUM_SCENEGRAPH_LIBRARIES}
	${MAGNUM_APPLICATION_LIBRARIES}
	${MAGNUM_PRIMITIVES_LIBRARIES})
