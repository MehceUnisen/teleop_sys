project(cpp_srv_cli)

cmake_minimum_required(VERSION 3.10)


file(GLOB PROJECT_SOURCES
    *.h
    */*.h
    *.cpp
    */*.cpp
    )
message(STATUS "PROJECT_SOURCES:\n${PROJECT_SOURCES}")

find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb")
set(THREADS_PREFER_PTHREAD_FLAG OFF)

include_directories(SYSTEM /usr/include/c++/7)
add_executable(tcp-echo-client ${PROJECT_SOURCES})

target_link_libraries(tcp-echo-client Threads::Threads )


