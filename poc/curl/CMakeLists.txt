cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake)

project(CurlExample)

find_package(Boost REQUIRED COMPONENTS asio beast system url)
find_package(OpenSSL REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_path(JWT_CPP_INCLUDE_DIRS "jwt-cpp/base.h" HINTS ${CMAKE_CURRENT_LIST_DIR}/vcpkg/installed/${VCPKG_TARGET_TRIPLET}/include)

add_executable(server server.cpp)
target_include_directories(server PRIVATE ${JWT_CPP_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR})
target_link_libraries(server ${Boost_LIBRARIES} OpenSSL::SSL OpenSSL::Crypto)
