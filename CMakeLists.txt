cmake_minimum_required(VERSION 2.8.7)  # Поверка версии cmake
project(Transformer)

# Назначим переменные для сборки проекта
set(DEBUG_FLAGS "-g -Og -pipe -Wall -pedantic -std=gnu++11")
set(RELEASE_FLAGS "-O2 -pipe -march=native -std=gnu++11")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# Определяем параметры сборки
set(prog_name transformer)
set(CMAKE_CXX_FLAGS ${RELEASE_FLAGS})
set(CMAKE_C_FLAGS ${RELEASE_FLAGS})
set(CMAKE_CXX_FLAGS_DEBUG ${DEBUG_FLAGS})
set(CMAKE_C_FLAGS_DEBUG ${DEBUG_FLAGS})
set(CMAKE_CONFIGURATION_TYPES Debug Release)

add_subdirectory(build)
add_custom_target(run
    COMMAND ${prog_name}
    DEPENDS ${prog_name}
    WORKING_DIRECTORY ./bin
)
