cmake_minimum_required(VERSION 3.20)
project(powerprompt)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(CMAKE_CXX_STANDARD 17)

add_executable(powerprompt program.cpp)
set_property(TARGET powerprompt PROPERTY CXX_STANDARD 20)
set_property(TARGET powerprompt PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(powerprompt ${CONAN_LIBS})

add_executable(powerprompt_tests tests.cpp)
set_property(TARGET powerprompt_tests PROPERTY CXX_STANDARD 20)
set_property(TARGET powerprompt_tests PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(powerprompt_tests ${CONAN_LIBS} )
