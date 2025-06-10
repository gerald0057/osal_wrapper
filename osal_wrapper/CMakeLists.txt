cmake_minimum_required(VERSION 3.10)
project(osal_wrapper)

set(CMAKE_C_STANDARD 11)

add_library(osal_wrapper STATIC
    osal_port_linux.c
)

target_include_directories(osal_wrapper PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

if(UNIX AND NOT APPLE)
    target_link_libraries(osal_wrapper PUBLIC pthread)
endif()

include(CMakePackageConfigHelpers)

install(TARGETS osal_wrapper
    EXPORT osal_wrapperTargets
    DESTINATION lib
)

install(EXPORT osal_wrapperTargets
    FILE osal_wrapperTargets.cmake
    NAMESPACE osal_wrapper::
    DESTINATION lib/cmake/osal_wrapper
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ DESTINATION include FILES_MATCHING PATTERN "*.h")

# 生成Config文件
configure_package_config_file(
    ${CMAKE_CURRENT_LIST_DIR}/cmake/osal_wrapperConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/osal_wrapperConfig.cmake
    INSTALL_DESTINATION lib/cmake/osal_wrapper
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/osal_wrapperConfigVersion.cmake
    VERSION 1.0.0
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/osal_wrapperConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/osal_wrapperConfigVersion.cmake
    DESTINATION lib/cmake/osal_wrapper
)