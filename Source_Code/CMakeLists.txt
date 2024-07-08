cmake_minimum_required(VERSION 3.0)

project(Local-ILP)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")


file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.cc")

set(INCLUDES ${PROJECT_SOURCE_DIR}/src)

add_executable(${PROJECT_NAME} ${SOURCES})

include_directories(${INCLUDES})

target_link_libraries(Local-ILP z -lz boost_date_time boost_system)