cmake_minimum_required(VERSION 3.21)

target_sources(apkfmt PRIVATE
    types.h
    res/manifest.cpp
    res/manifest.h
    res/ro.cpp
    res/ro.h
    arsc/verify.cpp
    arsc/verify.h
    arsc/axml_parser.cpp
    arsc/axml_parser.h
    arsc/axml_names.cpp
    arsc/stream.cpp
    arsc/stream.h
    xml/decoder.cpp
    xml/decoder.h
    repack.cpp
    repack.h
    holder.cpp
    holder.h
    main.cpp

    validate.cpp
    validate.h
)
target_link_libraries(apkfmt zip mbedcrypto Boost::property_tree Boost::locale Boost::program_options Boost::endian)
