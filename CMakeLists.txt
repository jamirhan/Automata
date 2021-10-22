cmake_minimum_required(VERSION 3.19)


set(PROJECT_NAME automata)
set(PROJECT_TEST_NAME automata_test)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)

SET(CMAKE_CXX_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage --coverage")
SET(CMAKE_C_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage --coverage")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")



include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)


set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(${PROJECT_TEST_NAME} Automata.h main_tests.cpp )

add_executable(${PROJECT_NAME} Automata.h main.cpp)

target_link_libraries(${PROJECT_TEST_NAME} gtest_main)

include(GoogleTest)
gtest_discover_tests(${PROJECT_TEST_NAME})

#add_test(main_tests.cpp)
