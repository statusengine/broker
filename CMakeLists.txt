cmake_minimum_required(VERSION 3.0)
project(statusengine-broker)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(MY_CXX_COMPILE_FEATURES cxx_generic_lambdas cxx_range_for cxx_strong_enums)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(FindPkgConfig)

find_package(PkgConfig REQUIRED)

pkg_check_modules(NAEMON REQUIRED naemon)
pkg_check_modules(GEARMAND REQUIRED gearmand)

add_library(statusengine SHARED
	nebmodule.cpp Statusengine.cpp
	GearmanClient.cpp LogStream.cpp
	NagiosObjects/NagiosObject.cpp
	NagiosObjects/NagiosHost.cpp NagiosObjects/NagiosHostStatusData.cpp NagiosObjects/NagiosHostCheckData.cpp
	NagiosObjects/NagiosService.cpp NagiosObjects/NagiosServiceStatusData.cpp NagiosObjects/NagiosServiceCheckData.cpp NagiosObjects/NagiosServiceCheckPerfData.cpp
	NagiosObjects/NagiosStateChangeData.cpp NagiosObjects/NagiosLogData.cpp
	NagiosCallbacks/HostStatusCallback.cpp NagiosCallbacks/HostCheckCallback.cpp
	NagiosCallbacks/ServiceStatusCallback.cpp NagiosCallbacks/ServiceCheckCallback.cpp
	NagiosCallbacks/StateChangeCallback.cpp NagiosCallbacks/LogDataCallback.cpp
)
target_include_directories(statusengine PUBLIC ${NAEMON_INCLUDE_DIRS} ${GEARMAND_INCLUDE_DIRS})
target_link_libraries(statusengine PUBLIC ${GEARMAND_LIBRARIES})

target_compile_options(statusengine PUBLIC ${NAEMON_CFLAGS_OTHER} ${GEARMAND_CFLAGS_OTHER})
target_compile_features(statusengine INTERFACE ${MY_CXX_COMPILE_FEATURES})
