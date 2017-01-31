project( sensor_unit )

find_package( OpenCV REQUIRED )

set( EXECUTABLE_OUTPUT_PATH ${PROJECT_BUILD_DIR} )

# Set the include Path
set( SENSOR_UNIT_INCLUDE_DIRS 
	.
	sensor_interfaces
	misc
)

include_directories( ${SENSOR_UNIT_INCLUDE_DIRS} )

set( SENSOR_UNIT_FILES
	main.cpp
	misc/i2c_access.cpp
	misc/logfile.cpp
	Sensor_Server.cpp
	Sensor_Server_working_thread.cpp
	Sensor_Server_analyse_functions.cpp
	sensor_interfaces/magnetometer_interface.cpp
	sensor_interfaces/magnetometer_lsm9ds1.cpp
	sensor_interfaces/gyroscope_interface.cpp
	sensor_interfaces/gyroscope_lsm9ds1.cpp
	misc/time_helper.cpp
)

add_executable( sensor_server ${SENSOR_UNIT_FILES} )
target_link_libraries(sensor_server server_inet ${OpenCV_LIBS} protocol_headers inet_udp xml_library )
add_dependencies( sensor_server server_inet inet_udp xml_library )

install (TARGETS sensor_server DESTINATION ${BINARY_INSTALL_DIR} )
