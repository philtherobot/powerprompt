cmake_minimum_required(VERSION 3.20)
project(powerprompt)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(CMAKE_CXX_STANDARD 17)

add_executable(powerprompt main.cpp)
target_link_libraries(powerprompt ${CONAN_LIBS})