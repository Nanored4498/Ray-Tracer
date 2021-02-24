project(RT)

cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra -pthread -std=c++17")
# set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra -pthread -ggdb -g -pg -std=c++17")
file(GLOB SOURCES "src/*.cpp" "src/*.c")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_executable(${PROJECT_NAME} ${SOURCES})