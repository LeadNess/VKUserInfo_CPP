cmake_minimum_required(VERSION 3.15)
project(vk-tracker)

set(CMAKE_CXX_STANDARD 20)

add_executable(vk-tracker src/main.cpp src/api.h src/api.cpp  src/json.h src/interface.h
        src/VKGraph.h src/VKGraph.cpp src/VKUserInfo.h src/VKUserInfo.cpp
        src/VKUserAnalyzer.h src/VKUserAnalyzer.cpp src/VKUsersRelation.h src/VKUsersRelation.cpp)

add_subdirectory(cpr)
include_directories(${CPR_INCLUDE_DIRS})
target_link_libraries(vk-tracker ${CPR_LIBRARIES})
