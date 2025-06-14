cmake_minimum_required(VERSION 3.30)
project(autostruct)

set(CMAKE_CXX_STANDARD 20)

# Устанавливаем сборку по умолчанию в Release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Добавляем флаг для больших объектников, решает проблему relocation truncated
if(MINGW)
    add_compile_options(-Wa,-mbig-obj)
endif()

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

set(AUTOSTRUCT_LIB_SRC
        lib/client/client.cpp
        lib/server/server.cpp
        app/models/user_info.h
        app/models/log_entry.h
        app/models/product.h
        app/models/transaction.h
        app/models/analytics_event.h
        app/models/device_info.h
        app/models/weather_data.h
)

add_library(autostructlib STATIC ${AUTOSTRUCT_LIB_SRC})

target_include_directories(autostructlib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/lib
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/client
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/server
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/core
        ${httplib_SOURCE_DIR}
)

target_link_libraries(autostructlib PUBLIC
        nlohmann_json::nlohmann_json
)

if(WIN32)
    target_compile_definitions(autostructlib PRIVATE
            _WIN32_WINNT=0x0A00
            NOMINMAX
    )
    target_link_libraries(autostructlib PRIVATE ws2_32)
elseif(UNIX)
    target_link_libraries(autostructlib PRIVATE pthread)
endif()

add_executable(autostruct
        app/main.cpp
)

target_include_directories(autostruct PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/app
)

target_link_libraries(autostruct PRIVATE
        autostructlib
)
