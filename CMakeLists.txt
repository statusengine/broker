cmake_minimum_required(VERSION 3.0)
project(statusengine-broker)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_INCLUDE_CURRENT_DIR ON)


option(CMAKE_POSITION_INDEPENDENT_CODE "Use position independent code" ON)
option(WITH_GEARMAN "Add gearman message queue support" ON)
option(WITH_RABBITMQ "Add rabbitmq message queue support" ON)

include(FindPkgConfig)
include(ExternalProject)

find_package(PkgConfig REQUIRED)

ExternalProject_Add(project_json-c
  GIT_REPOSITORY https://github.com/json-c/json-c.git
  PREFIX ${PROJECT_SOURCE_DIR}/vendor/json-c
  INSTALL_DIR ${PROJECT_SOURCE_DIR}/vendor/json-c/dist
  CMAKE_ARGS -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=${PROJECT_SOURCE_DIR}/vendor/json-c/dist -DCMAKE_POSITION_INDEPENDENT_CODE=${CMAKE_POSITION_INDEPENDENT_CODE} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  UPDATE_COMMAND ""
  BUILD_COMMAND cmake --build . --target json-c -- -j
  INSTALL_COMMAND cmake --build . --target install/fast -- -j
  TEST_COMMAND ""
)

ExternalProject_Add(project_yaml_cpp
  GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
  PREFIX ${PROJECT_SOURCE_DIR}/vendor/yaml_cpp
  INSTALL_DIR ${PROJECT_SOURCE_DIR}/vendor/yaml_cpp/dist
  CMAKE_ARGS -DYAML_BUILD_SHARED_LIBS=OFF -DYAML_CPP_BUILD_TOOLS=OFF -DYAML_CPP_BUILD_CONTRIB=OFF -DYAML_CPP_BUILD_TESTS=OFF -DCMAKE_INSTALL_PREFIX=${PROJECT_SOURCE_DIR}/vendor/yaml_cpp/dist -DCMAKE_POSITION_INDEPENDENT_CODE=${CMAKE_POSITION_INDEPENDENT_CODE} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  UPDATE_COMMAND ""
  TEST_COMMAND ""
)

add_library(json-c STATIC IMPORTED)
set_property(TARGET json-c PROPERTY IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/vendor/json-c/dist/lib/libjson-c.a)
add_dependencies(json-c project_json-c)

set(JSONC_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/vendor/json-c/dist/include/json-c)

add_library(yaml_cpp STATIC IMPORTED)
set_property(TARGET yaml_cpp PROPERTY IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/vendor/yaml_cpp/dist/lib/libyaml-cpp.a)
add_dependencies(yaml_cpp project_yaml_cpp)

set(YAML_CPP_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/vendor/yaml_cpp/dist/include/yaml-cpp)

set (SESources
	Nebmodule.cpp Statusengine.cpp
	MessageHandler/MessageHandlerList.cpp
)

pkg_check_modules(UCHARDET REQUIRED uchardet)
pkg_check_modules(GEARMAND gearmand)
pkg_check_modules(RABBITMQC librabbitmq)
pkg_check_modules(GLIB REQUIRED glib-2.0)

if(WITH_GEARMAN AND NOT GEARMAND_FOUND)
	message(SEND_ERROR "WITH_GEARMAN is ON, but could not find gearman")
endif(WITH_GEARMAN AND NOT GEARMAND_FOUND)

if(NOT RABBITMQC_FOUND AND WITH_RABBITMQ)
	message(SEND_ERROR "WITH_RABBITMQ is ON, but could not find librabbitmq")
endif(NOT RABBITMQC_FOUND AND WITH_RABBITMQ)


if(WITH_GEARMAN)
	list(APPEND SESources MessageHandler/GearmanClient.cpp)
endif(WITH_GEARMAN)

if(WITH_RABBITMQ)
	list(APPEND SESources MessageHandler/RabbitmqClient.cpp)
endif(WITH_RABBITMQ)


add_library(statusengine SHARED ${SESources})


if(WITH_GEARMAN)
	target_include_directories(statusengine PRIVATE ${GEARMAND_INCLUDE_DIRS})
	target_link_libraries(statusengine PRIVATE ${GEARMAND_LIBRARIES})
	target_compile_options(statusengine PRIVATE ${GEARMAND_CFLAGS_OTHER})
	add_definitions(-DWITH_GEARMAN)
endif(WITH_GEARMAN)

if(WITH_RABBITMQ)
	target_include_directories(statusengine PRIVATE ${RABBITMQC_INCLUDE_DIRS})
	target_link_libraries(statusengine PRIVATE ${RABBITMQC_LIBRARIES})
	target_compile_options(statusengine PRIVATE ${RABBITMQC_CFLAGS_OTHER})
	add_definitions(-DWITH_RABBITMQ)
	if(RABBITMQC_VERSION VERSION_LESS "0.6.0")
		add_definitions(-DWITH_RABBITMQ_CX060)
	endif(RABBITMQC_VERSION VERSION_LESS "0.6.0")
	if(RABBITMQC_VERSION VERSION_LESS "0.8.0")
		add_definitions(-DWITH_RABBITMQ_CX080)
	endif(RABBITMQC_VERSION VERSION_LESS "0.8.0")
endif(WITH_RABBITMQ)

target_include_directories(statusengine PRIVATE ${YAML_CPP_INCLUDE_DIRS})
target_link_libraries(statusengine PRIVATE yaml_cpp)

target_include_directories(statusengine PRIVATE ${JSONC_INCLUDE_DIRS})
target_link_libraries(statusengine PRIVATE json-c)

target_include_directories(statusengine PRIVATE ${GLIB_INCLUDE_DIRS})
target_link_libraries(statusengine PRIVATE ${GLIB_LIBRARIES})
target_compile_options(statusengine PRIVATE ${GLIB_CFLAGS_OTHER})

target_include_directories(statusengine PRIVATE ${UCHARDET_INCLUDE_DIRS})
target_link_libraries(statusengine PRIVATE ${UCHARDET_LIBRARIES})
target_compile_options(statusengine PRIVATE ${UCHARDET_CFLAGS_OTHER})

if(BUILD_NAGIOS)
	if(NAGIOS_INCLUDE_DIR)
		target_include_directories(statusengine PRIVATE ${NAGIOS_INCLUDE_DIR})
	endif(NAGIOS_INCLUDE_DIR)

	target_link_libraries(statusengine PRIVATE uuid event gearman)
	add_definitions(-DBUILD_NAGIOS)
	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		target_compile_options(statusengine PRIVATE -Wno-pointer-arith)
	endif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
else(BUILD_NAGIOS)
	pkg_check_modules(NAEMON REQUIRED naemon)
	target_include_directories(statusengine PRIVATE ${NAEMON_INCLUDE_DIRS})
	target_compile_options(statusengine PRIVATE ${NAEMON_CFLAGS_OTHER})
endif(BUILD_NAGIOS)

install(TARGETS statusengine LIBRARY DESTINATION lib)
