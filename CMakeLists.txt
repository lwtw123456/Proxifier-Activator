cmake_minimum_required(VERSION 3.21)

project(Proxifier_Activator LANGUAGES CXX)

if(NOT WIN32)
    message(FATAL_ERROR "Windows only")
endif()

add_executable(Proxifier_Activator WIN32
    main.cpp
    keygen.hpp
)

target_compile_features(Proxifier_Activator PRIVATE cxx_std_23)

target_link_libraries(Proxifier_Activator PRIVATE
    advapi32
    user32
)

if(MSVC)
    target_compile_options(Proxifier_Activator PRIVATE /utf-8)
endif()
