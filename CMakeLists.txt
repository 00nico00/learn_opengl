cmake_minimum_required(VERSION 3.26)
project(opengl_learn)

set(CMAKE_CXX_STANDARD 23)

include_directories(./include)

set(SOURCES
        main.cpp
        include/logger.hpp
)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

find_package(glad CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glad::glad)