project( joystick_node )

set( EXECUTABLE_OUTPUT_PATH ${PROJECT_BUILD_DIR} )

# Set the include Path
set( JOYSTICK_NODE_INCLUDE_DIRS 
	.
	${gpio_INCLUDE_DIRECTORIES}
)

include_directories( ${JOYSTICK_NODE_INCLUDE_DIRS} )


set( JOYSTICK_NODE_FILES
	main.cpp
)

add_executable( joystick_node ${JOYSTICK_NODE_FILES} )
target_link_libraries(joystick_node socket_inet)
add_dependencies( joystick_node gpio_headders )

install (TARGETS joystick_node DESTINATION ${BINARY_INSTALL_DIR} )
