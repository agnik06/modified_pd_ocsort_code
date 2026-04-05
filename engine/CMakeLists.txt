cmake_minimum_required(VERSION 2.8)
set(LIB_NAME "PetDetector_v1.camera.samsung")
project(${LIB_NAME})
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++14" COMPILER_SUPPORTS_CXX14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread")

set(BUILD_x86_TESTER TRUE)

add_definitions(-DBUILD_x86)
add_definitions(-DTESTING)
add_definitions(-DTEMPAVG)
add_definitions( -DDLINTERFACELIBLOCATION_SYSTEM="libDLInterface.so")
add_definitions( -D__PET_OD_VERSION__="v1.0-pc")
add_definitions( -DLIBBUILDMODE="default")

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/third_party/DLInterface/include)
include_directories(${CMAKE_SOURCE_DIR}/third_party/json)
include_directories(${CMAKE_SOURCE_DIR}/third_party/Eigen/include)
include_directories(${CMAKE_SOURCE_DIR}/third_party/Eigen/src)
# include_directories(${CMAKE_SOURCE_DIR}/third_party/snap_libs/include)

include_directories(${CMAKE_SOURCE_DIR}/include/object_detector)
include_directories(${CMAKE_SOURCE_DIR}/src/core)
include_directories(${CMAKE_SOURCE_DIR}/src/modules/detector/tracker_wrapper/tracker)
include_directories(${CMAKE_SOURCE_DIR}/src)

add_library(DLInterface SHARED IMPORTED)
set_property(TARGET DLInterface PROPERTY IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/third_party/DLInterface/libs/x86_64/libDLInterface.so")

file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp" "${CMAKE_SOURCE_DIR}/src/*/*.cpp" "${CMAKE_SOURCE_DIR}/src/*/*/*.cpp" "${CMAKE_SOURCE_DIR}/src/*/*/*/*.ccc" "${CMAKE_SOURCE_DIR}/src/*/*/*/*/*.cc")

IF(BUILD_x86_TESTER)
	message("Set to build test exectuable")
    add_executable(${LIB_NAME} ${SOURCES} test/test.cpp)
ELSE()
	message("Set to build .so library")
	add_library(${LIB_NAME} SHARED ${SOURCES})	
ENDIF(BUILD_x86_TESTER)

target_link_libraries(${LIB_NAME} ${OpenCV_LIBS} ${CMAKE_DL_LIBS})

string(ASCII 27 Esc)
set(ColorReset "${Esc}[m")
set(Green   "${Esc}[32m")

IF(BUILD_x86_TESTER)
    install(TARGETS ${LIB_NAME} DESTINATION ${CMAKE_SOURCE_DIR}/bin/x86_64)
    message("${Green}Build Completed | Output directory: ./bin/x86_64${ColorReset}")
ELSE()
    install(TARGETS ${LIB_NAME} DESTINATION ${CMAKE_SOURCE_DIR}/libs/x86_64)
    message("${Green}Build Completed | Output directory: ./libs/x86_64${ColorReset}")
ENDIF(BUILD_x86_TESTER)
