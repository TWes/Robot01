cmake_minimum_required(VERSION 2.8)


set( PROJECT_BUILD_DIR 
	~/Robot01/build/builds
)

set( LIB_INSTALL_DIR
	~/Robot01/install/libraries
)

set( BINARY_INSTALL_DIR
	~/Robot01/install/
)

set( CMAKE_CXX_FLAGS
	${CMAKE_CXX_FLAGS}
	" -std=gnu++11 "
)

# Build the connecton libraries
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/../source/shared/connection_library
	${CMAKE_CURRENT_SOURCE_DIR}/source/shared/connection_library
)

# Build the joystic node
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/../source/shared/joystick_node
	${CMAKE_CURRENT_SOURCE_DIR}/source/shared/joystick_node
)

# Build the GPIO Server
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/../source/inside/gpio
	${CMAKE_CURRENT_SOURCE_DIR}/source/inside/gpio
)

# Build the Control Server
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/../source/inside/control
	${CMAKE_CURRENT_SOURCE_DIR}/source/inside/control
)

# Build the Robot Monitor
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/../source/remote/robot_monitor
	${CMAKE_CURRENT_SOURCE_DIR}/source/remote/robot_monitor
)
