cmake_minimum_required(VERSION 3.11)

set(CMAKE_CXX_STANDARD 11 CACHE STRING "The C++ standard to use")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(WITH_GEARMAN "enable gearman" OFF)
option(WITH_RABBITMQ "enable rabbitmq" OFF)
option(NAGIOS_INCLUDE_DIR "path to nagios include dir" OFF)
option(NAEMON_INCLUDE_DIR "path to naemon include dir" OFF)

if(${CMAKE_VERSION} VERSION_LESS 3.14)
    macro(FetchContent_MakeAvailable NAME)
        FetchContent_GetProperties(${NAME})
        if(NOT ${NAME}_POPULATED)
            FetchContent_Populate(${NAME})
            add_subdirectory(${${NAME}_SOURCE_DIR} ${${NAME}_BINARY_DIR})
        endif()
    endmacro()
endif()

find_package(PkgConfig REQUIRED)
include(FetchContent)

project(
  statusengine
  VERSION 4.2
  LANGUAGES CXX
)

pkg_check_modules(UCHARDET REQUIRED uchardet)
pkg_check_modules(JSONC REQUIRED json-c)
set(STATUSENGINE_LIBS ${UCHARDET_LIBRARIES} ${JSONC_LIBRARIES})
set(STATUSENGINE_INCLUDES ${UCHARDET_INCLUDE_DIRS} ${JSONC_INCLUDE_DIRS})

if(WITH_GEARMAN)
    pkg_check_modules(GEARMAN REQUIRED gearmand)
    set(STATUSENGINE_LIBS ${GEARMAN_LIBRARIES} ${STATUSENGINE_LIBS})
    set(STATUSENGINE_INCLUDES ${GEARMAN_INCLUDE_DIRS} ${STATUSENGINE_INCLUDES})
endif()
if(WITH_RABBITMQ)
    pkg_check_modules(RABBITMQ REQUIRED librabbitmq)
    set(STATUSENGINE_LIBS ${RABBITMQ_LIBRARIES} ${STATUSENGINE_LIBS})
    set(STATUSENGINE_INCLUDES ${RABBITMQ_INCLUDE_DIRS} ${STATUSENGINE_INCLUDES})
endif()

if(NAGIOS_INCLUDE_DIR)
    set(STATUSENGINE_INCLUDES ${NAGIOS_INCLUDE_DIR} ${STATUSENGINE_INCLUDES})
endif()

if(NAEMON_INCLUDE_DIR)
set(STATUSENGINE_INCLUDES ${NAEMON_INCLUDE_DIR} ${STATUSENGINE_INCLUDES})
endif()

if(NOT NAGIOS_INCLUDE_DIR AND NOT NAEMON_INCLUDE_DIR)
    pkg_check_modules(NAEMON REQUIRED naemon)
    set(STATUSENGINE_INCLUDES ${NAEMON_INCLUDE_DIRS} ${STATUSENGINE_INCLUDES})
endif()

if(NOT NAGIOS_INCLUDE_DIR)
    pkg_check_modules(GLIB2 REQUIRED glib-2.0)
    set(STATUSENGINE_LIBS ${GLIB2_LIBRARIES} ${STATUSENGINE_LIBS})
    set(STATUSENGINE_INCLUDES ${GLIB2_INCLUDE_DIRS} ${STATUSENGINE_INCLUDES})
endif()


set(toml11_BUILD_TEST OFF CACHE INTERNAL "")
FetchContent_Declare(
  toml11
  GIT_REPOSITORY https://github.com/ToruNiina/toml11.git
  GIT_TAG        v3.7.1
)

FetchContent_GetProperties(toml11)
if(NOT toml11_POPULATED)
  FetchContent_Populate(toml11)
  add_subdirectory(${toml11_SOURCE_DIR} ${toml11_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

set(STATUSENGINE_INCLUDES ${toml11_SOURCE_DIR} ${toml11_SOURCE_DIR}/toml ${STATUSENGINE_INCLUDES})

add_subdirectory(src)

