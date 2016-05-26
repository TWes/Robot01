project( cam )

find_package( OpenCV REQUIRED )

set( EXECUTABLE_OUTPUT_PATH ${PROJECT_BUILD_DIR} )

# Set the include Path
set( CAM_INCLUDE_DIRS 
	.
)

include_directories( ${CAM_INCLUDE_DIRS} )

set( CAM_FILES
	main.cpp
)

add_executable( cam_server ${CAM_FILES} )
target_link_libraries(cam_server server_inet ${OpenCV_LIBS})
add_dependencies( cam_server server_inet )

install (TARGETS cam_server DESTINATION ${BINARY_INSTALL_DIR} )
