cmake_minimum_required(VERSION 3.11)

set(CMAKE_CXX_STANDARD 17 CACHE STRING "The C++ standard to use")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(WITH_GEARMAN "enable gearman" OFF)
option(WITH_RABBITMQ "enable rabbitmq" OFF)
option(NAGIOS_INCLUDE_DIR "path to nagios include dir" OFF)
option(NAEMON_INCLUDE_DIR "path to naemon include dir" OFF)
option(ENABLE_TESTS "enable testing" OFF)
option(NAEMON_CORE_BUILD "built naemon as external dependency" OFF)
option(NAEMON_CORE_BUILD_VERSION "set version of naemon to build" "v1.4.0")
option(ENABLE_CLANG_TIDY "enable clang tidy for build" OFF)

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
include(ExternalProject)

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
    add_definitions(-DWITH_GEARMAN)
endif()
if(WITH_RABBITMQ)
    pkg_check_modules(RABBITMQ REQUIRED librabbitmq)
    set(STATUSENGINE_LIBS ${RABBITMQ_LIBRARIES} ${STATUSENGINE_LIBS})
    set(STATUSENGINE_INCLUDES ${RABBITMQ_INCLUDE_DIRS} ${STATUSENGINE_INCLUDES})
    add_definitions(-DWITH_RABBITMQ)
endif()

if(NOT NAEMON_CORE_BUILD)
  if(NAGIOS_INCLUDE_DIR)
    set(STATUSENGINE_INCLUDES ${NAGIOS_INCLUDE_DIR} ${STATUSENGINE_INCLUDES})
  endif()

  if(NAEMON_INCLUDE_DIR)
    set(STATUSENGINE_INCLUDES ${NAEMON_INCLUDE_DIR} ${STATUSENGINE_INCLUDES})
  endif()

  if(NOT NAGIOS_INCLUDE_DIR AND NOT NAEMON_INCLUDE_DIR)
      pkg_check_modules(NAEMON REQUIRED naemon)
      link_directories(${NAEMON_LIBRARY_DIRS})
      set(STATUSENGINE_LIBS ${NAEMON_LIBRARIES} ${STATUSENGINE_LIBS})
      set(STATUSENGINE_INCLUDES ${NAEMON_INCLUDE_DIRS} ${STATUSENGINE_INCLUDES})
  endif()
endif()

if(NOT NAGIOS_INCLUDE_DIR)
  pkg_check_modules(GLIB2 REQUIRED glib-2.0)
  set(STATUSENGINE_LIBS ${GLIB2_LIBRARIES} ${STATUSENGINE_LIBS})
  set(STATUSENGINE_INCLUDES ${GLIB2_INCLUDE_DIRS} ${STATUSENGINE_INCLUDES})
endif()

if(NAEMON_CORE_BUILD)
  ExternalProject_Add(
    naemon-core
    GIT_REPOSITORY https://github.com/naemon/naemon-core.git
    GIT_TAG ${NAEMON_CORE_BUILD_VERSION}
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ./autogen.sh --prefix=<INSTALL_DIR>
    BUILD_COMMAND make -j
    INSTALL_COMMAND make install
    BUILD_IN_SOURCE true
  )
  ExternalProject_Get_Property(naemon-core INSTALL_DIR)
  link_directories(${INSTALL_DIR}/lib)
  set(STATUSENGINE_LIBS naemon ${STATUSENGINE_LIBS})
  set(STATUSENGINE_INCLUDES ${INSTALL_DIR}/include/ ${STATUSENGINE_INCLUDES})
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

FetchContent_Declare(
  wise_enum
  GIT_REPOSITORY https://github.com/quicknir/wise_enum.git
  GIT_TAG        3.0.0
)
FetchContent_GetProperties(wise_enum)
FetchContent_MakeAvailable(wise_enum)
set(STATUSENGINE_INCLUDES ${wise_enum_SOURCE_DIR} ${STATUSENGINE_INCLUDES})


ExternalProject_Add(
  rapidjson
  GIT_REPOSITORY https://github.com/Tencent/rapidjson.git
  GIT_TAG v1.1.0
  UPDATE_COMMAND ""
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  GIT_SUBMODULES ""
)
ExternalProject_Get_Property(rapidjson SOURCE_DIR)
set(STATUSENGINE_INCLUDES ${SOURCE_DIR}/include ${STATUSENGINE_INCLUDES})

FetchContent_Declare(backward
        GIT_REPOSITORY https://github.com/bombela/backward-cpp
        GIT_TAG v1.6)
FetchContent_MakeAvailable(backward)

if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

set(INSTALL_GTEST OFF)
set(BUILD_GMOCK ON)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_library(statusengine SHARED ${BACKWARD_ENABLE})
add_backward(statusengine)
add_dependencies(statusengine rapidjson)
if(NAEMON_CORE_BUILD)
  add_dependencies(statusengine naemon-core)
endif()

add_subdirectory(src)

target_link_libraries(statusengine PRIVATE ${STATUSENGINE_LIBS})
target_include_directories(statusengine SYSTEM PRIVATE ${STATUSENGINE_INCLUDES})
target_include_directories(statusengine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(ENABLE_TESTS)
  enable_testing(true)
  include(CTest)
  add_subdirectory(tests)
endif()

if(ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_COMMAND NAMES clang-tidy)

    if(NOT CLANG_TIDY_COMMAND)
        message(ERROR "CMake_RUN_CLANG_TIDY is ON but clang-tidy is not found!")
        set(CMAKE_CXX_CLANG_TIDY "" CACHE STRING "" FORCE)
    else()
        message(STATUS "CMake_RUN_CLANG_TIDY is ON")
        set(CLANGTIDY_EXTRA_ARGS
            "-extra-arg=-Wno-unknown-warning-option"
    )
    endif()

    set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND};-p=${CMAKE_BINARY_DIR};${CLANGTIDY_EXTRA_ARGS}" CACHE STRING "" FORCE)

    add_custom_target(clang-tidy
      COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ${CMAKE_PROJECT_NAME}
      COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target clang-tidy
      COMMENT "Running clang-tidy..."
    )
endif()

find_program(CLANG_FORMAT_EXE "clang-format")
mark_as_advanced(FORCE CLANG_FORMAT_EXE)
if(CLANG_FORMAT_EXE)
  message(STATUS "clang-format found: ${CLANG_FORMAT_EXE}")
else()
  message(STATUS "clang-format not found!")
endif()

function(clang_format TARGET_NAME)
  if(CLANG_FORMAT_EXE)
    set(FORMAT_FILES)
    # Check through the ARGN's, determine existent files
    foreach(item IN LISTS ARGN)
      if(TARGET ${item})
        # If the item is a target, then we'll attempt to grab the associated
        # source files from it.
        get_target_property(_TARGET_TYPE ${item} TYPE)
        if(NOT _TARGET_TYPE STREQUAL "INTERFACE_LIBRARY")
          get_property(
            _TEMP
            TARGET ${item}
            PROPERTY SOURCES)
          foreach(iter IN LISTS _TEMP)
            if(EXISTS ${iter})
              set(FORMAT_FILES ${FORMAT_FILES} ${iter})
            endif()
          endforeach()
        endif()
      elseif(EXISTS ${item})
        # Check if it's a full file path
        set(FORMAT_FILES ${FORMAT_FILES} ${item})
      elseif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${item})
        # Check if it's based on the current source dir
        set(FORMAT_FILES ${FORMAT_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/${item})
      endif()
    endforeach()

    # Make the target
    if(FORMAT_FILES)
      if(NOT TARGET ${TARGET_NAME})
        add_custom_target(${TARGET_NAME} COMMAND ${CLANG_FORMAT_EXE} -i
                                                 -style=file ${FORMAT_FILES})
      endif()
    endif()

  endif()
endfunction()

clang_format(format statusengine tests benchmarks)
