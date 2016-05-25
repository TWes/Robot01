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

add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/../source/Shared/connection_library
	${CMAKE_CURRENT_SOURCE_DIR}/source/Shared/connection_library
)

# Build the Robot Monitor
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/../source/Remote/Robot_Monitor
	${CMAKE_CURRENT_SOURCE_DIR}/source/Remote/Robot_Monitor
)
