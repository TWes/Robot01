project( robot_monitor )

#find_package(Qt5 REQUIRED QtGui QtCore)
find_package( Qt5Widgets )

set( EXECUTABLE_OUTPUT_PATH ${PROJECT_BUILD_DIR} )


# Set the include Path
set( ROBOT_MONITOR_INCLUDE_DIRS 
	.
	${Qt5Widgets_INCLUDE_DIRS}
)

add_definitions(${Qt5Widgets_DEFINITIONS})

include_directories( ${ROBOT_MONITOR_INCLUDE_DIRS} )

set(CMAKE_AUTOMOC ON)

set( ROBOT_MONITOR_FILES
	main.cpp
	MainWindow.cpp
	SensorConnection.cpp
	DialogWindows.cpp
	GraphContainer.cpp
	GraphPlotter.cpp
	connectionStatus.cpp
)

add_executable( robot_monitor ${ROBOT_MONITOR_FILES} )
target_link_libraries(robot_monitor Qt5::Widgets socket_inet connection_library protocol_headers inet_udp)

install (TARGETS robot_monitor DESTINATION ${BINARY_INSTALL_DIR} )
