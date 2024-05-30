cmake_minimum_required(VERSION 3.10)
project(CurlExample)

set(CMAKE_CXX_STANDARD 17)

# Find Boost libraries
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/../vcpkg/scripts/buildsystems/vcpkg.cmake)
set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "${CMAKE_SOURCE_DIR}/../vcpkg/installed/x64-linux/include")
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${CMAKE_SOURCE_DIR}/../vcpkg/installed/x64-linux/lib")

find_package(Boost REQUIRED COMPONENTS system)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
endif()

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

include_directories(${CMAKE_SOURCE_DIR})


add_subdirectory(wazuh-http-request)

# Add the server executable
add_executable(server server.cpp)
target_link_libraries(server Boost::system ${Boost_LIBRARIES} ${ZLIB_LIBRARIES} OpenSSL::SSL OpenSSL::Crypto urlrequest)

# Add the client executable
add_executable(client client.cpp)
target_link_libraries(client ${CURL_LIBRARIES} ${ZLIB_LIBRARIES} OpenSSL::SSL OpenSSL::Crypto urlrequest)
