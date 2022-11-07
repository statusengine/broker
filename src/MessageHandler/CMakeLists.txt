if(WITH_GEARMAN)
    target_sources(statusengine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/GearmanClient.cpp)
endif()

if(WITH_RABBITMQ)
    target_sources(statusengine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/RabbitmqClient.cpp)
endif()

target_sources(statusengine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/MessageHandlerList.cpp)

target_include_directories(statusengine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})