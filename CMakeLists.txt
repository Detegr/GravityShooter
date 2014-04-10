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
	DebugTools
	Text
	FreeTypeFont)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CORRADE_CXX_FLAGS}")
include_directories(${MAGNUM_INCLUDE_DIRS} ${MAGNUM_APPLICATION_INCLUDE_DIRS})

include_directories(BEFORE ${CMAKE_CURRENT_BINARY_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
	${CMAKE_CURRENT_BINARY_DIR}/configure.h)

corrade_add_resource(shaderdata src/shaders/shaders.conf)
corrade_add_resource(fontdata src/fonts/fonts.conf)

add_executable(gravity
	src/main.cpp \\
	src/plane.cpp \\
	src/ball.cpp \\
	src/loaders.cpp \\
	src/pointlight.cpp \\
	src/directionallight.cpp \\
	src/shaders/pointlightshader.cpp \\
	src/shaders/directionallightshader.cpp \\
	src/shaders/deferred_1stpass.cpp \\
	${shaderdata}
	${fontdata})

target_link_libraries(gravity
	${MAGNUM_LIBRARIES}
	${MAGNUM_MESHTOOLS_LIBRARIES}
	${MAGNUM_SHADERS_LIBRARIES}
	${MAGNUM_SCENEGRAPH_LIBRARIES}
	${MAGNUM_APPLICATION_LIBRARIES}
	${MAGNUM_PRIMITIVES_LIBRARIES}
	${MAGNUM_TEXT_LIBRARIES})
