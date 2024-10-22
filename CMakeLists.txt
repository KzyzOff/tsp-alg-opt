cmake_minimum_required(VERSION 3.28)
project(TSP)

set(CMAKE_CXX_STANDARD 20)

set(HEADERS_DIR "${PROJECT_SOURCE_DIR}/include")
set(SOURCES_DIR "${PROJECT_SOURCE_DIR}/src")
set(
    SOURCES
        ${SOURCES_DIR}/utils.cpp
        ${SOURCES_DIR}/CSVLogger.cpp
        ${SOURCES_DIR}/TSPSolver.cpp
        ${SOURCES_DIR}/Graph.cpp
        ${SOURCES_DIR}/Greedy.cpp
        ${SOURCES_DIR}/Loader.cpp
        ${SOURCES_DIR}/PopulationManager.cpp
)
set(
    HEADERS
        ${HEADERS_DIR}/utils.hpp
        ${HEADERS_DIR}/constants.hpp
        ${HEADERS_DIR}/CSVLogger.hpp
        ${HEADERS_DIR}/TSPSolver.hpp
        ${HEADERS_DIR}/Graph.hpp
        ${HEADERS_DIR}/Greedy.hpp
        ${HEADERS_DIR}/Loader.hpp
        ${HEADERS_DIR}/PopulationManager.hpp
)

add_executable(${PROJECT_NAME}
        main.cpp
        ${SOURCES}
        ${HEADERS}
)

target_include_directories(${PROJECT_NAME} PUBLIC ${HEADERS_DIR})
