cmake_minimum_required(VERSION 3.10)

project(osal_wrapper)

set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SRC
    src/main.c
)

if(UNIX AND NOT APPLE)
    message(STATUS "Build for Linux platform")
    list(APPEND SRC src/osal_port_linux.c)
endif()

add_executable(osal_wrapper ${SRC})

if(UNIX AND NOT APPLE)
    target_link_libraries(osal_wrapper pthread)
endif()