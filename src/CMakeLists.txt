cmake_minimum_required(VERSION 3.21)

add_executable(apkfmt)
target_sources(apkfmt PRIVATE
    types.h
    res/manifest.cpp
    res/manifest.h
    res/ro.cpp
    res/ro.h
    arsc/verify.cpp
    arsc/verify.h
    repack.cpp
    repack.h
    holder.cpp
    holder.h
    main.cpp

    validate.cpp
    validate.h
)
include_directories(.)
target_link_libraries(apkfmt zip mbedcrypto)
