project( gpio )

set( EXECUTABLE_OUTPUT_PATH ${PROJECT_BUILD_DIR} )

# Set the include Path
set( GPIO_INCLUDE_DIRS 
	.
)

include_directories( ${GPIO_INCLUDE_DIRS} )

set( GPIO_FILES
	main.cpp
	GPIO_Interface.cpp
	Steering.cpp
)

add_executable( gpio_server ${GPIO_FILES} )
target_link_libraries(gpio_server server_inet socket_inet)

install (TARGETS gpio_server DESTINATION ${BINARY_INSTALL_DIR} )
