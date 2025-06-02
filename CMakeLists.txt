cmake_minimum_required(VERSION 3.30)
project(sockobjserver)

set(CMAKE_CXX_STANDARD 20)

# Включение автоматического скачивания зависимостей
include(FetchContent)

# nlohmann/json
FetchContent_Declare(
        json
        URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
)
FetchContent_MakeAvailable(json)

# cpp-httplib
FetchContent_Declare(
        httplib
        URL https://github.com/yhirose/cpp-httplib/archive/refs/tags/v0.15.3.tar.gz
)
FetchContent_MakeAvailable(httplib)

# Основной исполняемый файл
add_executable(sockobjserver
        main.cpp
        server.cpp
        client.cpp
)

# Подключение зависимостей
target_link_libraries(sockobjserver PRIVATE
        nlohmann_json::nlohmann_json
)

# Пути к заголовкам
target_include_directories(sockobjserver PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${httplib_SOURCE_DIR}
)

# Для Windows
if(WIN32)
    target_compile_definitions(sockobjserver PRIVATE
            _WIN32_WINNT=0x0A00
            NOMINMAX  # Отключает min/max макросы Windows
    )
    target_link_libraries(sockobjserver PRIVATE ws2_32)
endif()

# Для Linux
if(UNIX)
    target_link_libraries(sockobjserver PRIVATE pthread)
endif()