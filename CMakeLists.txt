cmake_minimum_required(VERSION 3.0)
project(statusengine-broker)
#set (CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # -std=c++11 instead of -std=gnu++11
set(MY_CXX_COMPILE_FEATURES cxx_generic_lambdas cxx_range_for cxx_strong_enums)

include(FindPkgConfig)

find_package(PkgConfig REQUIRED)

pkg_check_modules(NAEMON REQUIRED naemon)

add_library(statusengine SHARED nebmodule.cpp statusengine.cpp)
target_include_directories(statusengine PUBLIC ${NAEMON_INCLUDE_DIRS})
target_compile_options(statusengine PUBLIC ${NAEMON_CFLAGS_OTHER})
target_compile_features(statusengine INTERFACE ${MY_CXX_COMPILE_FEATURES})