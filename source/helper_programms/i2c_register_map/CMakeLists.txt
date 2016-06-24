project( i2c_register_map )

find_package(Qt4 REQUIRED QtGui QtCore)

set( EXECUTABLE_OUTPUT_PATH ${PROJECT_BUILD_DIR} )


# Set the include Path
set( I2C_REGISTER_MAP_INCLUDE_DIRS 
	.
)

include_directories( ${I2C_REGISTER_MAP_INCLUDE_DIRS} )

set(CMAKE_AUTOMOC ON)

set( I2C_REGISTER_MAP_FILES
	main.cpp
	RegisterMap.cpp
	RegisterMapper.cpp
	MainWindow.cpp
	ControlWidget.cpp
	TableWidget.cpp
)

add_executable( i2c_register_map ${I2C_REGISTER_MAP_FILES} )
target_link_libraries(i2c_register_map Qt4::QtGui Qt4::QtCore socket_inet protocol_headers inet_udp)

install (TARGETS i2c_register_map DESTINATION ${BINARY_INSTALL_DIR} )
