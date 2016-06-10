project( connection_library )


set( LIBRARY_OUTPUT_PATH ${PROJECT_BUILD_DIR} )

# Set the include Path
set( LIB_INCLUDE_DIRS 
	.
)

include_directories( ${LIB_INCLUDE_DIRS} )


#Set the include files
set( LIB_INCLUDE_FILES
	server_inet.hpp
	socket_inet.hpp
	server_inet_udp.hpp
	socket_inet_udp.hpp
)

install (FILES ${LIB_INCLUDE_FILES} DESTINATION ${LIB_INSTALL_DIR})

#########################
## Server Inet Library ##
#########################

set( SERVER_INET_FILES
	server_inet.cpp
)

add_library( server_inet ${SERVER_INET_FILES} )
install (TARGETS server_inet DESTINATION ${LIB_INSTALL_DIR} )
target_include_directories( server_inet PUBLIC ${LIB_INCLUDE_DIRS} )


#########################
## socket inet library ##
#########################

set( SOCKET_INET_FILES
	socket_inet.cpp
)

add_library( socket_inet ${SOCKET_INET_FILES} )
target_include_directories( socket_inet PUBLIC ${LIB_INCLUDE_DIRS} )
install (TARGETS socket_inet DESTINATION ${LIB_INSTALL_DIR} )


#############################
## socket inet udp library ##
#############################

set( SOCKET_INET_FILES
	socket_inet_udp.cpp
)

add_library( socket_inet_udp ${SOCKET_INET_FILES} )
target_include_directories( socket_inet_udp PUBLIC ${LIB_INCLUDE_DIRS} )
install (TARGETS socket_inet_udp DESTINATION ${LIB_INSTALL_DIR} )


#############################
## Server Inet Library udp ##
#############################

set( SOCKET_INET_FILES
	server_inet_udp.cpp
)

add_library( server_inet_udp ${SOCKET_INET_FILES} )
target_include_directories( server_inet_udp PUBLIC ${LIB_INCLUDE_DIRS} )
install (TARGETS server_inet_udp DESTINATION ${LIB_INSTALL_DIR} )

