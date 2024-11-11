
cmake_minimum_required(VERSION 3.12) # cmake version here
project(yaecs VERSION 1.0 LANGUAGES CXX) # Your project name here

option(YAECS_TEST "Build test application" ON)


set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

file(GLOB_RECURSE INCLUDES CONFIGURE_DEPENDS "${INCLUDE_DIR}/*.h" "${INCLUDE_DIR}/*.hpp")
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${SOURCE_DIR}/*.c" "${SOURCE_DIR}/*.cpp")


add_library(yaecs INTERFACE)
target_include_directories(yaecs INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_library(yaecs_lib STATIC "${INCLUDES}" "${SOURCES}")
target_include_directories(yaecs_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(yaecs_lib PROPERTIES LINKER_LANGUAGE CXX)

include_directories("${INCLUDE_DIR}")
 
if(${YAECS_TEST})
    set(TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/test")
    file(GLOB_RECURSE TEST CONFIGURE_DEPENDS "${TEST_DIR}/*.c" "${TEST_DIR}/*.cpp")

    add_executable(yaecs_test "${TEST}")
    target_compile_features(yaecs_test PRIVATE cxx_std_20)
    target_link_libraries(yaecs_test PRIVATE yaecs_lib)
endif()
