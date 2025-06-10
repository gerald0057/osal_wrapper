cmake_minimum_required(VERSION 3.10)

find_package(osal_wrapper REQUIRED)
add_executable(osal_example main.c)
target_link_libraries(osal_example PRIVATE osal_wrapper::osal_wrapper)
