cmake_minimum_required(VERSION 3.21)

project(Apkfmt)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE -Ofast)

add_subdirectory(external/zip)

set(ENABLE_PROGRAMS OFF CACHE BOOL "Build Mbed TLS programs." FORCE)
set(UNSAFE_BUILD OFF CACHE BOOL "Allow unsafe builds. These builds ARE NOT SECURE." FORCE)
set(ENABLE_TESTING OFF CACHE BOOL "Build Mbed TLS tests." FORCE)
add_subdirectory(external/mbedtls)
include_directories(external/mbedtls/include)
target_compile_options(mbedcrypto PRIVATE -Wno-everything)

add_subdirectory(external/boost)

include_directories(src)
add_executable(apkfmt)
add_subdirectory(src)

add_custom_command(
    TARGET apkfmt POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_SOURCE_DIR}/android_attributes_list.txt"
    "$<TARGET_FILE_DIR:apkfmt>/android_attributes_list.txt"
)

