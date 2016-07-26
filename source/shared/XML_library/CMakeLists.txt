project( XML_library )


set( LIBRARY_OUTPUT_PATH ${PROJECT_BUILD_DIR} )

# Set the include Path
set( LIB_INCLUDE_DIRS 
	.
)

include_directories( ${LIB_INCLUDE_DIRS} )


#Set the include files
set( LIB_INCLUDE_FILES
	XMLElement.hpp
	XMLWriter.hpp
)

install (FILES ${LIB_INCLUDE_FILES} DESTINATION ${LIB_INSTALL_DIR})

#########################
## XML_library ##
#########################

set( XML_LIB_FILES
	XMLElement.cpp
	XMLWriter.cpp
)

add_library( xml_library ${XML_LIB_FILES} )
install (TARGETS xml_library DESTINATION ${LIB_INSTALL_DIR} )
target_include_directories( xml_library PUBLIC ${LIB_INCLUDE_DIRS} )