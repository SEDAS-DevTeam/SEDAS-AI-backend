cmake_minimum_required(VERSION 3.15)
project(sedas_ai_backend)

set(CMAKE_CXX_STANDARD 17)

add_executable(test ./src/test.cpp)
target_link_libraries(test PRIVATE curl ncurses)