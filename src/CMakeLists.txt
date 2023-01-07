file(GLOB STATUSENGINE_SOURCES *.cpp)

if(BUILD_NAGIOS)
    list(REMOVE_ITEM STATUSENGINE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/NebmoduleNaemon.cpp)
else()
    list(REMOVE_ITEM STATUSENGINE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/NebmoduleNagios.cpp)
endif()

add_library(statusengine SHARED ${STATUSENGINE_SOURCES})

add_subdirectory(MessageHandler)

target_link_libraries(statusengine PRIVATE ${STATUSENGINE_LIBS})
target_include_directories(statusengine SYSTEM PRIVATE ${STATUSENGINE_INCLUDES})
target_include_directories(statusengine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

install(
    TARGETS statusengine
)