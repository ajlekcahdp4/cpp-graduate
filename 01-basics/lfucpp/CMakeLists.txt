cmake_minimum_required(VERSION 3.23)

add_compile_options(-g --verbose)

project(lfucpp)

set (LFUCPP_RESOURCES
    lfu-cache.cc    
)

add_executable(lfucpp ${LFUCPP_RESOURCES})




find_package(GTest)
enable_testing()

find_program(BASH_PROGRAM bash)
if(BASH_PROGRAM)
    add_test(NAME TestLFU.testEndToEnd COMMAND ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/test.sh "$<TARGET_FILE:lfucpp>" ${CMAKE_CURRENT_SOURCE_DIR})
endif()