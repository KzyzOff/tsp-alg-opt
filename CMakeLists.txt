cmake_minimum_required(VERSION 3.18)
project(TSP)

set(CMAKE_CXX_STANDARD 20)
if ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
    set(CMAKE_CXX_FLAGS "-Wall -Wextra")
endif ()

if ( NOT WIN32 )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")
endif ()

set(HEADERS_DIR "${PROJECT_SOURCE_DIR}/include")
set(SOURCES_DIR "${PROJECT_SOURCE_DIR}/src")
set(
    SOURCES
        ${SOURCES_DIR}/utils.cpp
        ${SOURCES_DIR}/CSVLogger.cpp
        ${SOURCES_DIR}/TSPSolver.cpp
        ${SOURCES_DIR}/Population.cpp
        ${SOURCES_DIR}/selectors/TournamentSelector.cpp
#        ${SOURCES_DIR}/selectors/RouletteSelector.cpp
        ${SOURCES_DIR}/cross_operators/OXOperator.cpp
        ${SOURCES_DIR}/cross_operators/PMXOperator.cpp
        ${SOURCES_DIR}/Graph.cpp
#        ${SOURCES_DIR}/Greedy.cpp
        ${SOURCES_DIR}/Loader.cpp
        ${SOURCES_DIR}/PopulationManager.cpp
)
set(
    HEADERS
        ${HEADERS_DIR}/utils.hpp
        ${HEADERS_DIR}/constants.hpp
        ${HEADERS_DIR}/types.hpp
        ${HEADERS_DIR}/CSVLogger.hpp
        ${HEADERS_DIR}/TSPSolver.hpp
        ${HEADERS_DIR}/Population.hpp
        ${HEADERS_DIR}/selectors/Selector.hpp
        ${HEADERS_DIR}/selectors/TournamentSelector.hpp
#        ${HEADERS_DIR}/selectors/RouletteSelector.hpp
        ${HEADERS_DIR}/cross_operators/CrossOperator.hpp
        ${HEADERS_DIR}/cross_operators/OXOperator.hpp
        ${HEADERS_DIR}/cross_operators/PMXOperator.hpp
        ${HEADERS_DIR}/Graph.hpp
#        ${HEADERS_DIR}/Greedy.hpp
        ${HEADERS_DIR}/Loader.hpp
        ${HEADERS_DIR}/PopulationManager.hpp
)

add_executable(${PROJECT_NAME}
        main.cpp
        ${SOURCES}
        ${HEADERS}
)

set(EXTERNAL_LIB_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/external")

target_include_directories(${PROJECT_NAME} PUBLIC ${HEADERS_DIR} ${EXTERNAL_LIB_INCLUDE_DIR})

# Download JSON library
file(
    DOWNLOAD https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp
    ${EXTERNAL_LIB_INCLUDE_DIR}/json.hpp
)

target_include_directories(${PROJECT_NAME} PUBLIC ${EXTERNAL_LIB_INCLUDE_DIR})
