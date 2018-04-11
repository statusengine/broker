cmake_minimum_required(VERSION 3.0)
project(statusengine-broker)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(MY_CXX_COMPILE_FEATURES cxx_generic_lambdas cxx_range_for cxx_strong_enums)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(FindPkgConfig)

find_package(PkgConfig REQUIRED)

add_library(statusengine SHARED
	nebmodule.cpp Statusengine.cpp
	GearmanClient.cpp LogStream.cpp Configuration.cpp
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

pkg_check_modules(GEARMAND REQUIRED gearmand)
target_include_directories(statusengine PUBLIC ${GEARMAND_INCLUDE_DIRS})
target_link_libraries(statusengine PUBLIC ${GEARMAND_LIBRARIES})
target_compile_options(statusengine PUBLIC ${GEARMAND_CFLAGS_OTHER})


if(DEFINED BUILD_NAGIOS)
	if(DEFINED NAGIOS_INCLUDE_DIR)
		target_include_directories(statusengine PUBLIC ${NAGIOS_INCLUDE_DIR})
	endif(DEFINED NAGIOS_INCLUDE_DIR)
	target_link_libraries(statusengine PUBLIC uuid event gearman)
	#target_compile_options(statusengine PUBLIC -luuid -levent -lgearman -ljson-c)
	add_definitions(-DBUILD_NAGIOS)
        if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
            target_compile_options(statusengine PUBLIC -Wno-pointer-arith)
        endif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
else(DEFINED BUILD_NAGIOS)
	pkg_check_modules(NAEMON REQUIRED naemon)
	target_include_directories(statusengine PUBLIC ${NAEMON_INCLUDE_DIRS})
	target_compile_options(statusengine PUBLIC ${NAEMON_CFLAGS_OTHER})
endif(DEFINED BUILD_NAGIOS)

target_compile_features(statusengine INTERFACE ${MY_CXX_COMPILE_FEATURES})

install(TARGETS statusengine LIBRARY DESTINATION lib)
