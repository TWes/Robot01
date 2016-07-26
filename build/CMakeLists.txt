cmake_minimum_required(VERSION 2.8)


set( PROJECT_BUILD_DIR 
	${CMAKE_CURRENT_LIST_DIR}/builds/
)

set( LIB_INSTALL_DIR
	${CMAKE_CURRENT_LIST_DIR}/../install/libraries/
)

set( BINARY_INSTALL_DIR
	${CMAKE_CURRENT_LIST_DIR}/../install
)

set( CMAKE_CXX_FLAGS
	${CMAKE_CXX_FLAGS}
	" -std=gnu++11 -pedantic-errors "
)

##########################
## Build shared targets ##
##########################

# Build the connecton libraries
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/../source/shared/connection_library
	${CMAKE_CURRENT_SOURCE_DIR}/source/shared/connection_library
)

# Build the joystic node
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/../source/shared/joystick_node
	${CMAKE_CURRENT_SOURCE_DIR}/source/shared/joystick_node
)

# Build the microcontroller
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/../source/shared/micro_controller
	${CMAKE_CURRENT_SOURCE_DIR}/source/shared/micro_controller
)

# Build the protocol headers
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/../source/shared/protocol_headers
	${CMAKE_CURRENT_SOURCE_DIR}/source/shared/protocol_headers
)

# Build the XML library
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/../source/shared/XML_library
	${CMAKE_CURRENT_SOURCE_DIR}/source/shared/XML_library
)

##########################
## Build inside targets ##
##########################

# Build the GPIO Server
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/../source/inside/gpio
	${CMAKE_CURRENT_SOURCE_DIR}/source/inside/gpio
)

# Build the Control Server
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/../source/inside/control
	${CMAKE_CURRENT_SOURCE_DIR}/source/inside/control
)

# Build the Sensor Server
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/../source/inside/sensor_unit
	${CMAKE_CURRENT_SOURCE_DIR}/source/inside/sensor_unit
)


##########################
## Build remote targets ##
##########################
# Build the Robot Monitor
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/../source/remote/robot_monitor
	${CMAKE_CURRENT_SOURCE_DIR}/source/remote/robot_monitor
)

###########################
## Build helper programs ##
###########################
# Build the i2c register map
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/../source/helper_programms/i2c_register_map
	${CMAKE_CURRENT_SOURCE_DIR}/source/helper_programms/i2c_register_map
)

###########################
## add real clean target ##
###########################
add_custom_target( hard_clean )
add_custom_command( TARGET hard_clean
	COMMAND rm -rf ../install builds CMakeFiles source
	COMMAND rm -f CMakeCache.txt cmake_install.cmake install_manifest.txt Makefile *~
)

