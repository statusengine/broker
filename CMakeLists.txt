cmake_minimum_required(VERSION 3.0)
project(statusengine-broker)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(MY_CXX_COMPILE_FEATURES cxx_generic_lambdas cxx_range_for cxx_strong_enums)

include(FindPkgConfig)

find_package(PkgConfig REQUIRED)

pkg_check_modules(NAEMON REQUIRED naemon)
pkg_check_modules(GEARMAND REQUIRED gearmand)

add_library(statusengine SHARED nebmodule.cpp NebmoduleCallback.cpp HostStatusCallback.cpp Statusengine.cpp NagiosObject.cpp NagiosHost.cpp GearmanClient.cpp LogStream.cpp)
target_include_directories(statusengine PUBLIC ${NAEMON_INCLUDE_DIRS} ${GEARMAND_INCLUDE_DIRS})
target_link_libraries(statusengine PUBLIC ${GEARMAND_LIBRARIES})

target_compile_options(statusengine PUBLIC ${NAEMON_CFLAGS_OTHER} ${GEARMAND_CFLAGS_OTHER})
target_compile_features(statusengine INTERFACE ${MY_CXX_COMPILE_FEATURES})
