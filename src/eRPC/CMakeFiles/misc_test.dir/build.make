# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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
CMAKE_SOURCE_DIR = /home/s1513031/eRPC

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/s1513031/eRPC

# Include any dependencies generated for this target.
include CMakeFiles/misc_test.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/misc_test.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/misc_test.dir/flags.make

CMakeFiles/misc_test.dir/tests/util_tests/misc_test.cc.o: CMakeFiles/misc_test.dir/flags.make
CMakeFiles/misc_test.dir/tests/util_tests/misc_test.cc.o: tests/util_tests/misc_test.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/s1513031/eRPC/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/misc_test.dir/tests/util_tests/misc_test.cc.o"
	g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/misc_test.dir/tests/util_tests/misc_test.cc.o -c /home/s1513031/eRPC/tests/util_tests/misc_test.cc

CMakeFiles/misc_test.dir/tests/util_tests/misc_test.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/misc_test.dir/tests/util_tests/misc_test.cc.i"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/s1513031/eRPC/tests/util_tests/misc_test.cc > CMakeFiles/misc_test.dir/tests/util_tests/misc_test.cc.i

CMakeFiles/misc_test.dir/tests/util_tests/misc_test.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/misc_test.dir/tests/util_tests/misc_test.cc.s"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/s1513031/eRPC/tests/util_tests/misc_test.cc -o CMakeFiles/misc_test.dir/tests/util_tests/misc_test.cc.s

CMakeFiles/misc_test.dir/tests/util_tests/misc_test.cc.o.requires:

.PHONY : CMakeFiles/misc_test.dir/tests/util_tests/misc_test.cc.o.requires

CMakeFiles/misc_test.dir/tests/util_tests/misc_test.cc.o.provides: CMakeFiles/misc_test.dir/tests/util_tests/misc_test.cc.o.requires
	$(MAKE) -f CMakeFiles/misc_test.dir/build.make CMakeFiles/misc_test.dir/tests/util_tests/misc_test.cc.o.provides.build
.PHONY : CMakeFiles/misc_test.dir/tests/util_tests/misc_test.cc.o.provides

CMakeFiles/misc_test.dir/tests/util_tests/misc_test.cc.o.provides.build: CMakeFiles/misc_test.dir/tests/util_tests/misc_test.cc.o


# Object files for target misc_test
misc_test_OBJECTS = \
"CMakeFiles/misc_test.dir/tests/util_tests/misc_test.cc.o"

# External object files for target misc_test
misc_test_EXTERNAL_OBJECTS =

build/misc_test: CMakeFiles/misc_test.dir/tests/util_tests/misc_test.cc.o
build/misc_test: CMakeFiles/misc_test.dir/build.make
build/misc_test: build/liberpc.a
build/misc_test: /usr/lib/libgtest.a
build/misc_test: CMakeFiles/misc_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/s1513031/eRPC/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable build/misc_test"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/misc_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/misc_test.dir/build: build/misc_test

.PHONY : CMakeFiles/misc_test.dir/build

CMakeFiles/misc_test.dir/requires: CMakeFiles/misc_test.dir/tests/util_tests/misc_test.cc.o.requires

.PHONY : CMakeFiles/misc_test.dir/requires

CMakeFiles/misc_test.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/misc_test.dir/cmake_clean.cmake
.PHONY : CMakeFiles/misc_test.dir/clean

CMakeFiles/misc_test.dir/depend:
	cd /home/s1513031/eRPC && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/s1513031/eRPC /home/s1513031/eRPC /home/s1513031/eRPC /home/s1513031/eRPC /home/s1513031/eRPC/CMakeFiles/misc_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/misc_test.dir/depend

