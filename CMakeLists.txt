cmake_minimum_required(VERSION 3.22)
project(programm)

set (CMAKE_CXX_STANDARD 23)
set (CXX_STANDARD 23)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG")
configure_file (paths.h.in ${CMAKE_CURRENT_SOURCE_DIR}/paths.h @ONLY)

add_subdirectory(lib/tinyxml2)

file(GLOB SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)

add_executable(${PROJECT_NAME} ${SRC_FILES} ${HEADER_FILES})

target_link_libraries(${PROJECT_NAME} tinyxml2)
