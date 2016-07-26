project( sensor_unit )

find_package( OpenCV REQUIRED )

set( EXECUTABLE_OUTPUT_PATH ${PROJECT_BUILD_DIR} )

# Set the include Path
set( SENSOR_UNIT_INCLUDE_DIRS 
	.
)

include_directories( ${SENSOR_UNIT_INCLUDE_DIRS} )

set( SENSOR_UNIT_FILES
	main.cpp
	i2c_access.cpp
	logfile.cpp
	Sensor_Server.cpp
	Sensor_Server_working_thread.cpp
	Sensor_Server_analyse_functions.cpp
	magnetometer_interface.cpp
	magnetometer_lsm9ds1.cpp
	time_helper.cpp
)

add_executable( sensor_server ${SENSOR_UNIT_FILES} )
target_link_libraries(sensor_server server_inet ${OpenCV_LIBS} protocol_headers inet_udp xml_library )
add_dependencies( sensor_server server_inet inet_udp xml_library )

install (TARGETS sensor_server DESTINATION ${BINARY_INSTALL_DIR} )
