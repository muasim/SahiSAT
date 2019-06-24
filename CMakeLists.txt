cmake_minimum_required(VERSION 3.9.0)

set(CMAKE_CXX_STANDARD 17)

project(SAHISAT C CXX)
set(DEPENDENCY_PATH ${CMAKE_SOURCE_DIR}/deps)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include_directories(src)

include_directories(${DEPENDENCY_PATH}/whereami-cpp/include)
add_library(whereami STATIC 
${DEPENDENCY_PATH}/whereami-cpp/source/whereami++.cpp
${DEPENDENCY_PATH}/whereami-cpp/source/whereami.c)
#MinGW workaround to set minimum required windows version
if(WIN32)
target_compile_definitions( whereami PUBLIC _WIN32_WINNT=_WIN32_WINNT_WINXP)
endif()

find_package(ASIO REQUIRED)
include_directories(${ASIO_ROOT})

find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})

########### SAHISAT ##############

# Populate a CMake variable with the sources
set(SAHISAT_SRC
src/main.cxx
src/TelemetryData.pb.cc)

add_executable(${CMAKE_PROJECT_NAME}  ${SAHISAT_SRC})
add_dependencies(${PROJECT_NAME} whereami)
target_link_libraries(${PROJECT_NAME} whereami)
target_link_libraries(${PROJECT_NAME} ASIO::ASIO)
target_link_libraries(${PROJECT_NAME} ${Protobuf_LIBRARIES})