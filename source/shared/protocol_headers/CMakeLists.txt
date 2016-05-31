project( protocol_headers )


set( LIBRARY_OUTPUT_PATH ${PROJECT_BUILD_DIR} )

# Set the include Path
set( PROTOCOL_LIB_INCLUDE_DIRS 
	.
)

include_directories( ${PROTOCOL_LIB_INCLUDE_DIRS} )

add_library( protocol_headers dummy_lib.cpp )
install (TARGETS protocol_headers DESTINATION ${LIB_INSTALL_DIR} )
target_include_directories( protocol_headers PUBLIC ${PROTOCOL_LIB_INCLUDE_DIRS} )
