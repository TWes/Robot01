# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/tim/Robot01/build

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/tim/Robot01/build

# Include any dependencies generated for this target.
include source/Shared/connection_library/CMakeFiles/server_inet.dir/depend.make

# Include the progress variables for this target.
include source/Shared/connection_library/CMakeFiles/server_inet.dir/progress.make

# Include the compile flags for this target's objects.
include source/Shared/connection_library/CMakeFiles/server_inet.dir/flags.make

source/Shared/connection_library/CMakeFiles/server_inet.dir/server_inet.cpp.o: source/Shared/connection_library/CMakeFiles/server_inet.dir/flags.make
source/Shared/connection_library/CMakeFiles/server_inet.dir/server_inet.cpp.o: /home/tim/Robot01/source/Shared/connection_library/server_inet.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/tim/Robot01/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object source/Shared/connection_library/CMakeFiles/server_inet.dir/server_inet.cpp.o"
	cd /home/tim/Robot01/build/source/Shared/connection_library && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/server_inet.dir/server_inet.cpp.o -c /home/tim/Robot01/source/Shared/connection_library/server_inet.cpp

source/Shared/connection_library/CMakeFiles/server_inet.dir/server_inet.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/server_inet.dir/server_inet.cpp.i"
	cd /home/tim/Robot01/build/source/Shared/connection_library && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/tim/Robot01/source/Shared/connection_library/server_inet.cpp > CMakeFiles/server_inet.dir/server_inet.cpp.i

source/Shared/connection_library/CMakeFiles/server_inet.dir/server_inet.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/server_inet.dir/server_inet.cpp.s"
	cd /home/tim/Robot01/build/source/Shared/connection_library && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/tim/Robot01/source/Shared/connection_library/server_inet.cpp -o CMakeFiles/server_inet.dir/server_inet.cpp.s

source/Shared/connection_library/CMakeFiles/server_inet.dir/server_inet.cpp.o.requires:
.PHONY : source/Shared/connection_library/CMakeFiles/server_inet.dir/server_inet.cpp.o.requires

source/Shared/connection_library/CMakeFiles/server_inet.dir/server_inet.cpp.o.provides: source/Shared/connection_library/CMakeFiles/server_inet.dir/server_inet.cpp.o.requires
	$(MAKE) -f source/Shared/connection_library/CMakeFiles/server_inet.dir/build.make source/Shared/connection_library/CMakeFiles/server_inet.dir/server_inet.cpp.o.provides.build
.PHONY : source/Shared/connection_library/CMakeFiles/server_inet.dir/server_inet.cpp.o.provides

source/Shared/connection_library/CMakeFiles/server_inet.dir/server_inet.cpp.o.provides.build: source/Shared/connection_library/CMakeFiles/server_inet.dir/server_inet.cpp.o

# Object files for target server_inet
server_inet_OBJECTS = \
"CMakeFiles/server_inet.dir/server_inet.cpp.o"

# External object files for target server_inet
server_inet_EXTERNAL_OBJECTS =

builds/libserver_inet.a: source/Shared/connection_library/CMakeFiles/server_inet.dir/server_inet.cpp.o
builds/libserver_inet.a: source/Shared/connection_library/CMakeFiles/server_inet.dir/build.make
builds/libserver_inet.a: source/Shared/connection_library/CMakeFiles/server_inet.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX static library ../../../builds/libserver_inet.a"
	cd /home/tim/Robot01/build/source/Shared/connection_library && $(CMAKE_COMMAND) -P CMakeFiles/server_inet.dir/cmake_clean_target.cmake
	cd /home/tim/Robot01/build/source/Shared/connection_library && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/server_inet.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
source/Shared/connection_library/CMakeFiles/server_inet.dir/build: builds/libserver_inet.a
.PHONY : source/Shared/connection_library/CMakeFiles/server_inet.dir/build

source/Shared/connection_library/CMakeFiles/server_inet.dir/requires: source/Shared/connection_library/CMakeFiles/server_inet.dir/server_inet.cpp.o.requires
.PHONY : source/Shared/connection_library/CMakeFiles/server_inet.dir/requires

source/Shared/connection_library/CMakeFiles/server_inet.dir/clean:
	cd /home/tim/Robot01/build/source/Shared/connection_library && $(CMAKE_COMMAND) -P CMakeFiles/server_inet.dir/cmake_clean.cmake
.PHONY : source/Shared/connection_library/CMakeFiles/server_inet.dir/clean

source/Shared/connection_library/CMakeFiles/server_inet.dir/depend:
	cd /home/tim/Robot01/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/tim/Robot01/build /home/tim/Robot01/source/Shared/connection_library /home/tim/Robot01/build /home/tim/Robot01/build/source/Shared/connection_library /home/tim/Robot01/build/source/Shared/connection_library/CMakeFiles/server_inet.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : source/Shared/connection_library/CMakeFiles/server_inet.dir/depend

