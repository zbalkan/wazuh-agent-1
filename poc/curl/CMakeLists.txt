cmake_minimum_required(VERSION 3.10)
project(CurlExample)

set(CMAKE_CXX_STANDARD 17)

# Find Boost libraries
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/../vcpkg/scripts/buildsystems/vcpkg.cmake)
set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "${CMAKE_SOURCE_DIR}/../vcpkg/installed/x64-linux/include")
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${CMAKE_SOURCE_DIR}/../vcpkg/installed/x64-linux/lib")

find_package(Boost REQUIRED COMPONENTS system url)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
endif()

# Add SQLite3 library
find_package(SQLite3 REQUIRED)
include_directories(${SQLite3_INCLUDE_DIRS})

set(RocksDB_DIR "${CMAKE_SOURCE_DIR}/../vcpkg/installed/x64-linux/share/rocksdb")
find_package(RocksDB REQUIRED)

# Find libcurl
find_package(CURL REQUIRED)
if(CURL_FOUND)
    include_directories(${CURL_INCLUDE_DIRS})
    link_directories(${CURL_LIBRARY_DIRS})
endif()

# Find zlib
find_package(ZLIB REQUIRED)
if(ZLIB_FOUND)
    include_directories(${ZLIB_INCLUDE_DIRS})
    link_directories(${ZLIB_LIBRARY_DIRS})
endif()

find_package(OpenSSL REQUIRED)

find_path(JWT_CPP_INCLUDE_DIRS "jwt-cpp/base.h" HINTS ${CMAKE_CURRENT_LIST_DIR}/../vcpkg/installed/${VCPKG_TARGET_TRIPLET}/include)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/wazuh_shared)
include_directories(${CMAKE_SOURCE_DIR}/external)

add_subdirectory(wazuh-http-request)

add_executable(server server.cpp)
add_executable(client client.cpp)

target_include_directories(server PRIVATE ${JWT_CPP_INCLUDE_DIRS})
target_include_directories(client PRIVATE ${JWT_CPP_INCLUDE_DIRS})

target_link_libraries(server Boost::system ${Boost_LIBRARIES} ${ZLIB_LIBRARIES} ${SQLite3_LIBRARIES} RocksDB::rocksdb OpenSSL::SSL OpenSSL::Crypto urlrequest )
target_link_libraries(client ${CURL_LIBRARIES} ${ZLIB_LIBRARIES} ${SQLite3_LIBRARIES} RocksDB::rocksdb OpenSSL::SSL OpenSSL::Crypto urlrequest )
