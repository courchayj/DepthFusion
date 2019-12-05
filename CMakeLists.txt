cmake_minimum_required(VERSION 2.6.0)

project(PixMeshFusion)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}")

find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenCV REQUIRED)
find_package(OpenNI2 REQUIRED)
find_package(GLUT REQUIRED)
include_directories(${OPENNI2_INCLUDE_DIR} ${OPENGL_INCLUDE_DIRS})
include_directories(${CMAKE_MODULE_PATH} ${GLEW_INCLUDE_DIRS})
include_directories( ${OPENGL_INCLUDE_DIRS} ${OPENCV_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS} )


add_definitions(-Dlinux=1)

set(CMAKE_CXX_FLAGS ${ADDITIONAL_CMAKE_CXX_FLAGS} "-O3 -msse2 -msse3 -Wall -std=c++11")

file(GLOB srcs *.cpp)
file(GLOB tools_srcs Tools/*.cpp)
file(GLOB utils_srcs Utils/*.cpp)
file(GLOB shader_srcs Shaders/*.*)

add_executable(PixMeshFusion 
               ${srcs}
               ${tools_srcs}
	       ${utils_srcs}
               ${shader_srcs}
)

target_link_libraries(PixMeshFusion
                      ${OPENGL_LIBRARY}
                      ${OPENNI2_LIBRARY}
                      ${OpenCV_LIBS}
                      ${GLUT_LIBRARY}
                      ${GLEW_LIBRARIES}
)


INSTALL(TARGETS PixMeshFusion
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

