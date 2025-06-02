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

add_executable(sockobjserver
        main.cpp
        server.cpp
        client.cpp
        router.h
        iroute_handler.h
        endpoint.h
        endpoint_registry.h
        serializable.h
        utils.h
)

target_link_libraries(sockobjserver PRIVATE
        nlohmann_json::nlohmann_json
)

target_include_directories(sockobjserver PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${httplib_SOURCE_DIR}
)

if(WIN32)
    target_compile_definitions(sockobjserver PRIVATE
            _WIN32_WINNT=0x0A00
            NOMINMAX
    )
    target_link_libraries(sockobjserver PRIVATE ws2_32)
endif()

if(UNIX)
    target_link_libraries(sockobjserver PRIVATE pthread)
endif()