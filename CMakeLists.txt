cmake_minimum_required(VERSION 3.28)
project(TSP)

set(CMAKE_CXX_STANDARD 17)

set(HEADERS_DIR "${PROJECT_SOURCE_DIR}/include")
set(SOURCES_DIR "${PROJECT_SOURCE_DIR}/src")
set(
    SOURCES
        ${SOURCES_DIR}/Loader.cpp
)

add_executable(${PROJECT_NAME}
        main.cpp
        ${SOURCES}
)

target_include_directories(${PROJECT_NAME} PUBLIC ${HEADERS_DIR})
