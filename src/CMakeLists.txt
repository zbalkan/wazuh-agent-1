cmake_minimum_required(VERSION 3.22)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")

project(wazuh-agent)

include(cmake/CommonSettings.cmake)
set_common_settings()

add_subdirectory(agent)
add_subdirectory(common)

if(WIN32)
    set(SOURCES
        agent/src/windows_service.cpp
        agent/src/windows_main.cpp
    )
elseif(UNIX)
    set(SOURCES
        agent/src/unix_daemon.cpp
        agent/src/unix_main.cpp
    )
endif()

list (APPEND SOURCES agent/src/process_options.cpp agent/src/register.cpp)

target_include_directories(Agent PUBLIC ${CMAKE_SOURCE_DIR}/include)
add_executable(wazuh-agent ${SOURCES})
target_link_libraries(wazuh-agent Agent Logger)

include(cmake/ConfigureTarget.cmake)
configure_target(Agent)

if(BUILD_TESTS)
    enable_testing()
endif()
