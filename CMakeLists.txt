cmake_minimum_required(VERSION 3.10)
project(secret CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-g -Wall -Werror")

add_executable(secret main.cpp)

add_library(slib STATIC version.cpp secret.cpp util/fs/fio.cpp util/math/cmathenhance.cpp)

target_link_libraries(secret slib)
