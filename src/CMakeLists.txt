file(GLOB STATUSENGINE_SOURCES *.cpp)

add_library(statusengine SHARED ${STATUSENGINE_SOURCES})

add_subdirectory(MessageHandler)

target_link_libraries(statusengine PRIVATE ${STATUSENGINE_LIBS})
target_include_directories(statusengine PUBLIC ${STATUSENGINE_INCLUDES})
target_include_directories(statusengine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
