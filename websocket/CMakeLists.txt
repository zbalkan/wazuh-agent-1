cmake_minimum_required(VERSION 3.10)
project(WebSocketExample)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set vcpkg toolchain file
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/../vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "Vcpkg toolchain file")
set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "${CMAKE_SOURCE_DIR}/../vcpkg/installed/x64-linux/include")
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${CMAKE_SOURCE_DIR}/../vcpkg/installed/x64-linux/lib")

# Find Boost libraries
find_package(Boost REQUIRED COMPONENTS system)
find_package(OpenSSL REQUIRED)

include_directories(${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})

# Add the executables
add_executable(server server.cpp)
add_executable(client client.cpp)

# Link libraries
target_link_libraries(server Boost::system OpenSSL::SSL OpenSSL::Crypto)
target_link_libraries(client Boost::system OpenSSL::SSL OpenSSL::Crypto)
