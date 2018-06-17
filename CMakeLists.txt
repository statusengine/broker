cmake_minimum_required(VERSION 3.0)
project(statusengine-broker)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(NOT DEFINED WITH_GEARMAN)
	set(WITH_GEARMAN ON)
endif(NOT DEFINED WITH_GEARMAN)

if(NOT DEFINED WITH_RABBITMQ)
	set(WITH_RABBITMQ ON)
endif(NOT DEFINED WITH_RABBITMQ)

include(FindPkgConfig)

find_package(PkgConfig REQUIRED)

set (SESources
	Nebmodule.cpp Statusengine.cpp LogStream.cpp
	Configuration.cpp RabbitmqConfiguration.cpp
	MessageHandler/MessageHandler.cpp MessageHandler/MessageHandlerList.cpp
	EventCallback/EventCallback.cpp EventCallback/BulkMessageCallback.cpp
	NagiosObjects/NagiosObject.cpp
	NagiosObjects/NagiosHost.cpp
	NagiosObjects/NagiosHostStatusData.cpp
	NagiosObjects/NagiosHostCheckData.cpp
	NagiosObjects/NagiosService.cpp
	NagiosObjects/NagiosServiceStatusData.cpp
	NagiosObjects/NagiosServiceCheckData.cpp
	NagiosObjects/NagiosServiceCheckPerfData.cpp
	NagiosObjects/NagiosStateChangeData.cpp
	NagiosObjects/NagiosLogData.cpp
	NagiosObjects/NagiosSystemCommandData.cpp
	NagiosObjects/NagiosCommentData.cpp
	NagiosCallbacks/NebmoduleCallback.cpp
	NagiosCallbacks/HostStatusCallback.cpp
	NagiosCallbacks/HostCheckCallback.cpp
	NagiosCallbacks/ServiceStatusCallback.cpp
	NagiosCallbacks/ServiceCheckCallback.cpp
	NagiosCallbacks/StateChangeCallback.cpp
	NagiosCallbacks/LogDataCallback.cpp
	NagiosCallbacks/SystemCommandDataCallback.cpp
	NagiosCallbacks/CommentDataCallback.cpp
	NagiosCallbacks/ExternalCommandDataCallback.cpp
	NagiosObjects/NagiosExternalCommandData.cpp
	NagiosCallbacks/AcknowledgementDataCallback.cpp
	NagiosObjects/NagiosAcknowledgementData.cpp
	NagiosCallbacks/FlappingDataCallback.cpp
	NagiosObjects/NagiosFlappingData.cpp
	NagiosCallbacks/DowntimeDataCallback.cpp
	NagiosObjects/NagiosDowntimeData.cpp
	NagiosCallbacks/NotificationDataCallback.cpp
	NagiosObjects/NagiosNotificationData.cpp
	NagiosCallbacks/ProgramStatusDataCallback.cpp
	NagiosObjects/NagiosProgramStatusData.cpp
	NagiosCallbacks/ContactStatusDataCallback.cpp
	NagiosObjects/NagiosContactStatusData.cpp
	NagiosCallbacks/ContactNotificationDataCallback.cpp
	NagiosObjects/NagiosContactNotificationData.cpp
	NagiosCallbacks/ContactNotificationMethodDataCallback.cpp
	NagiosObjects/NagiosContactNotificationMethodData.cpp
	NagiosCallbacks/EventHandlerDataCallback.cpp
	NagiosObjects/NagiosEventHandlerData.cpp
	NagiosCallbacks/ProcessDataCallback.cpp
	NagiosObjects/NagiosProcessData.cpp
)

pkg_check_modules(GEARMAND gearmand)
pkg_check_modules(RABBITMQC librabbitmq)
pkg_check_modules(ICUUC REQUIRED icu-uc)
pkg_check_modules(JSONC REQUIRED json-c)
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
	target_include_directories(statusengine PUBLIC ${GEARMAND_INCLUDE_DIRS})
	target_link_libraries(statusengine PUBLIC ${GEARMAND_LIBRARIES})
	target_compile_options(statusengine PUBLIC ${GEARMAND_CFLAGS_OTHER})
	add_definitions(-DWITH_GEARMAN)
endif(WITH_GEARMAN)

if(WITH_RABBITMQ)
	target_include_directories(statusengine PUBLIC ${RABBITMQC_INCLUDE_DIRS})
	target_link_libraries(statusengine PUBLIC ${RABBITMQC_LIBRARIES})
	target_compile_options(statusengine PUBLIC ${RABBITMQC_CFLAGS_OTHER})
	add_definitions(-DWITH_RABBITMQ)
	if(RABBITMQC_VERSION VERSION_LESS "0.6.0")
		add_definitions(-DWITH_RABBITMQ_CX060)
	endif(RABBITMQC_VERSION VERSION_LESS "0.6.0")
	if(RABBITMQC_VERSION VERSION_LESS "0.8.0")
		add_definitions(-DWITH_RABBITMQ_CX080)
	endif(RABBITMQC_VERSION VERSION_LESS "0.8.0")
endif(WITH_RABBITMQ)

target_include_directories(statusengine PUBLIC ${ICUUC_INCLUDE_DIRS})
target_link_libraries(statusengine PUBLIC ${ICUUC_LIBRARIES})
target_compile_options(statusengine PUBLIC ${ICUUC_CFLAGS_OTHER})

target_include_directories(statusengine PUBLIC ${JSONC_INCLUDE_DIRS})
target_link_libraries(statusengine PUBLIC ${JSONC_LIBRARIES})
target_compile_options(statusengine PUBLIC ${JSONC_CFLAGS_OTHER})

target_include_directories(statusengine PUBLIC ${GLIB_INCLUDE_DIRS})
target_link_libraries(statusengine PUBLIC ${GLIB_LIBRARIES})
target_compile_options(statusengine PUBLIC ${GLIB_CFLAGS_OTHER})

if(BUILD_NAGIOS)
	if(NAGIOS_INCLUDE_DIR)
		target_include_directories(statusengine PUBLIC ${NAGIOS_INCLUDE_DIR})
	endif(NAGIOS_INCLUDE_DIR)

	target_link_libraries(statusengine PUBLIC uuid event gearman)
	add_definitions(-DBUILD_NAGIOS)
	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		target_compile_options(statusengine PUBLIC -Wno-pointer-arith)
	endif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
else(BUILD_NAGIOS)
	pkg_check_modules(NAEMON REQUIRED naemon)
	target_include_directories(statusengine PUBLIC ${NAEMON_INCLUDE_DIRS})
	target_compile_options(statusengine PUBLIC ${NAEMON_CFLAGS_OTHER})
endif(BUILD_NAGIOS)

install(TARGETS statusengine LIBRARY DESTINATION lib)
