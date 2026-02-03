cmake_minimum_required(VERSION 3.20)
project(TheBlindNecromancer CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Core library
add_library(NecroCore
    NecroCore/Game.cpp
    NecroCore/Command.cpp
    NecroCore/Map.cpp
    NecroCore/CommandSerialization.h
    NecroCore/Game.h
    NecroCore/Command.h
    NecroCore/PulseResult.h
    NecroCore/MoveResult.h
    NecroCore/SummonResult.h
    NecroCore/Map.h
    NecroCore/Entity.h
)

target_include_directories(NecroCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/NecroCore
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty
)

add_executable(NecroClient
    NecroClient/NecroClient.cpp
)

target_include_directories(NecroClient PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/NecroCore
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty
)

target_link_libraries(NecroClient PRIVATE NecroCore)