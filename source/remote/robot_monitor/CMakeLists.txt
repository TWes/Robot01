project( robot_monitor )

find_package(Qt4 REQUIRED QtGui QtCore)

set( EXECUTABLE_OUTPUT_PATH ${PROJECT_BUILD_DIR} )


# Set the include Path
set( ROBOT_MONITOR_INCLUDE_DIRS 
	.
)

include_directories( ${ROBOT_MONITOR_INCLUDE_DIRS} )

set(CMAKE_AUTOMOC ON)

set( ROBOT_MONITOR_FILES
	main.cpp
	main_window.cpp
	map_widget.cpp
	Sensor_Connection.cpp
	status_widget.cpp
	output_box.cpp
	GraphPlotter.cpp
	graph_helper.cpp
)

add_executable( robot_monitor ${ROBOT_MONITOR_FILES} )
target_link_libraries(robot_monitor Qt4::QtGui Qt4::QtCore socket_inet protocol_headers inet_udp)

install (TARGETS robot_monitor DESTINATION ${BINARY_INSTALL_DIR} )
