cmake_minimum_required(VERSION 3.13)

project(redis-starter-cpp)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)

set(CMAKE_CXX_STANDARD 23) # Enable the C++23 standard
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)
#find_package(asio CONFIG REQUIRED)
# Locate standalone Asio headers
find_path(ASIO_INCLUDE_DIR asio.hpp)

if(NOT ASIO_INCLUDE_DIR)
    message(FATAL_ERROR "Could not find asio.hpp header!")
endif()

add_executable(redis ${SOURCE_FILES})

target_include_directories(redis PRIVATE
    ${ASIO_INCLUDE_DIR}
)

#target_link_libraries(redis PRIVATE asio asio::asio)
target_link_libraries(redis PRIVATE Threads::Threads)