# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.18

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/carl/workspace_v10/ADCS_Firmware/JTOK

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/carl/workspace_v10/ADCS_Firmware/JTOK/build

# Include any dependencies generated for this target.
include CMakeFiles/JTOK.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/JTOK.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/JTOK.dir/flags.make

CMakeFiles/JTOK.dir/src/jtok.c.o: CMakeFiles/JTOK.dir/flags.make
CMakeFiles/JTOK.dir/src/jtok.c.o: ../src/jtok.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/carl/workspace_v10/ADCS_Firmware/JTOK/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/JTOK.dir/src/jtok.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/JTOK.dir/src/jtok.c.o -c /home/carl/workspace_v10/ADCS_Firmware/JTOK/src/jtok.c

CMakeFiles/JTOK.dir/src/jtok.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/JTOK.dir/src/jtok.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/carl/workspace_v10/ADCS_Firmware/JTOK/src/jtok.c > CMakeFiles/JTOK.dir/src/jtok.c.i

CMakeFiles/JTOK.dir/src/jtok.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/JTOK.dir/src/jtok.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/carl/workspace_v10/ADCS_Firmware/JTOK/src/jtok.c -o CMakeFiles/JTOK.dir/src/jtok.c.s

# Object files for target JTOK
JTOK_OBJECTS = \
"CMakeFiles/JTOK.dir/src/jtok.c.o"

# External object files for target JTOK
JTOK_EXTERNAL_OBJECTS =

libJTOK.a: CMakeFiles/JTOK.dir/src/jtok.c.o
libJTOK.a: CMakeFiles/JTOK.dir/build.make
libJTOK.a: CMakeFiles/JTOK.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/carl/workspace_v10/ADCS_Firmware/JTOK/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library libJTOK.a"
	$(CMAKE_COMMAND) -P CMakeFiles/JTOK.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/JTOK.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/JTOK.dir/build: libJTOK.a

.PHONY : CMakeFiles/JTOK.dir/build

CMakeFiles/JTOK.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/JTOK.dir/cmake_clean.cmake
.PHONY : CMakeFiles/JTOK.dir/clean

CMakeFiles/JTOK.dir/depend:
	cd /home/carl/workspace_v10/ADCS_Firmware/JTOK/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/carl/workspace_v10/ADCS_Firmware/JTOK /home/carl/workspace_v10/ADCS_Firmware/JTOK /home/carl/workspace_v10/ADCS_Firmware/JTOK/build /home/carl/workspace_v10/ADCS_Firmware/JTOK/build /home/carl/workspace_v10/ADCS_Firmware/JTOK/build/CMakeFiles/JTOK.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/JTOK.dir/depend

