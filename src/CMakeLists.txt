file(GLOB STATUSENGINE_SOURCES *.cpp)

if(BUILD_NAGIOS)
    list(REMOVE_ITEM STATUSENGINE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/NebmoduleNaemon.cpp)
else()
    list(REMOVE_ITEM STATUSENGINE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/NebmoduleNagios.cpp)
endif()

target_sources(statusengine PRIVATE ${STATUSENGINE_SOURCES})

add_subdirectory(MessageHandler)

install(
    TARGETS statusengine
)