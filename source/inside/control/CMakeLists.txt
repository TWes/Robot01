project( control )

set( EXECUTABLE_OUTPUT_PATH ${PROJECT_BUILD_DIR} )

# Set the include Path
set( CONTROL_INCLUDE_DIRS 
	.
)

include_directories( ${CONTROL_INCLUDE_DIRS} )

set( CONTROL_FILES
	main.cpp
	Control_Server.cpp
)

add_executable( control_server ${CONTROL_FILES} )
target_link_libraries(control_server server_inet socket_inet)

install (TARGETS control_server DESTINATION ${BINARY_INSTALL_DIR} )
