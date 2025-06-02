cmake_minimum_required(VERSION 3.30)
project(sockobjserver)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(
        json
        URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
)
FetchContent_MakeAvailable(json)

FetchContent_Declare(
        httplib
        URL https://github.com/yhirose/cpp-httplib/archive/refs/tags/v0.15.3.tar.gz
)
FetchContent_MakeAvailable(httplib)

set(SOCKOBJ_LIB_SRC
        lib/client/client.cpp
        lib/server/server.cpp
)

add_library(sockobjlib STATIC
        ${SOCKOBJ_LIB_SRC}
)

target_include_directories(sockobjlib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/lib
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/client
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/server
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/core
        ${httplib_SOURCE_DIR}
)

target_link_libraries(sockobjlib PUBLIC
        nlohmann_json::nlohmann_json
)

if(WIN32)
    target_compile_definitions(sockobjlib PRIVATE
            _WIN32_WINNT=0x0A00
            NOMINMAX
    )
    target_link_libraries(sockobjlib PRIVATE ws2_32)
endif()

if(UNIX)
    target_link_libraries(sockobjlib PRIVATE pthread)
endif()

# Приложение
add_executable(sockobjserver
        app/main.cpp
        app/models/proxy.h
        app/models/simple_test.h
)

target_include_directories(sockobjserver PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/app
)

target_link_libraries(sockobjserver PRIVATE
        sockobjlib
)
