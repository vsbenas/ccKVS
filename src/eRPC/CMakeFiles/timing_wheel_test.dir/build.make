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
include CMakeFiles/timing_wheel_test.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/timing_wheel_test.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/timing_wheel_test.dir/flags.make

CMakeFiles/timing_wheel_test.dir/tests/util_tests/timing_wheel_test.cc.o: CMakeFiles/timing_wheel_test.dir/flags.make
CMakeFiles/timing_wheel_test.dir/tests/util_tests/timing_wheel_test.cc.o: tests/util_tests/timing_wheel_test.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/s1513031/eRPC/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/timing_wheel_test.dir/tests/util_tests/timing_wheel_test.cc.o"
	g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/timing_wheel_test.dir/tests/util_tests/timing_wheel_test.cc.o -c /home/s1513031/eRPC/tests/util_tests/timing_wheel_test.cc

CMakeFiles/timing_wheel_test.dir/tests/util_tests/timing_wheel_test.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/timing_wheel_test.dir/tests/util_tests/timing_wheel_test.cc.i"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/s1513031/eRPC/tests/util_tests/timing_wheel_test.cc > CMakeFiles/timing_wheel_test.dir/tests/util_tests/timing_wheel_test.cc.i

CMakeFiles/timing_wheel_test.dir/tests/util_tests/timing_wheel_test.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/timing_wheel_test.dir/tests/util_tests/timing_wheel_test.cc.s"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/s1513031/eRPC/tests/util_tests/timing_wheel_test.cc -o CMakeFiles/timing_wheel_test.dir/tests/util_tests/timing_wheel_test.cc.s

CMakeFiles/timing_wheel_test.dir/tests/util_tests/timing_wheel_test.cc.o.requires:

.PHONY : CMakeFiles/timing_wheel_test.dir/tests/util_tests/timing_wheel_test.cc.o.requires

CMakeFiles/timing_wheel_test.dir/tests/util_tests/timing_wheel_test.cc.o.provides: CMakeFiles/timing_wheel_test.dir/tests/util_tests/timing_wheel_test.cc.o.requires
	$(MAKE) -f CMakeFiles/timing_wheel_test.dir/build.make CMakeFiles/timing_wheel_test.dir/tests/util_tests/timing_wheel_test.cc.o.provides.build
.PHONY : CMakeFiles/timing_wheel_test.dir/tests/util_tests/timing_wheel_test.cc.o.provides

CMakeFiles/timing_wheel_test.dir/tests/util_tests/timing_wheel_test.cc.o.provides.build: CMakeFiles/timing_wheel_test.dir/tests/util_tests/timing_wheel_test.cc.o


# Object files for target timing_wheel_test
timing_wheel_test_OBJECTS = \
"CMakeFiles/timing_wheel_test.dir/tests/util_tests/timing_wheel_test.cc.o"

# External object files for target timing_wheel_test
timing_wheel_test_EXTERNAL_OBJECTS =

build/timing_wheel_test: CMakeFiles/timing_wheel_test.dir/tests/util_tests/timing_wheel_test.cc.o
build/timing_wheel_test: CMakeFiles/timing_wheel_test.dir/build.make
build/timing_wheel_test: build/liberpc.a
build/timing_wheel_test: /usr/lib/libgtest.a
build/timing_wheel_test: CMakeFiles/timing_wheel_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/s1513031/eRPC/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable build/timing_wheel_test"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/timing_wheel_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/timing_wheel_test.dir/build: build/timing_wheel_test

.PHONY : CMakeFiles/timing_wheel_test.dir/build

CMakeFiles/timing_wheel_test.dir/requires: CMakeFiles/timing_wheel_test.dir/tests/util_tests/timing_wheel_test.cc.o.requires

.PHONY : CMakeFiles/timing_wheel_test.dir/requires

CMakeFiles/timing_wheel_test.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/timing_wheel_test.dir/cmake_clean.cmake
.PHONY : CMakeFiles/timing_wheel_test.dir/clean

CMakeFiles/timing_wheel_test.dir/depend:
	cd /home/s1513031/eRPC && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/s1513031/eRPC /home/s1513031/eRPC /home/s1513031/eRPC /home/s1513031/eRPC /home/s1513031/eRPC/CMakeFiles/timing_wheel_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/timing_wheel_test.dir/depend

