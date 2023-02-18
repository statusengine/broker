file(GLOB STATUSENGINE_SOURCES *.cpp)

target_sources(statusengine PRIVATE ${STATUSENGINE_SOURCES})

add_subdirectory(MessageHandler)

install(
    TARGETS statusengine
)