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
include CMakeFiles/huge_alloc_test.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/huge_alloc_test.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/huge_alloc_test.dir/flags.make

CMakeFiles/huge_alloc_test.dir/tests/util_tests/huge_alloc_test.cc.o: CMakeFiles/huge_alloc_test.dir/flags.make
CMakeFiles/huge_alloc_test.dir/tests/util_tests/huge_alloc_test.cc.o: tests/util_tests/huge_alloc_test.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/s1513031/eRPC/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/huge_alloc_test.dir/tests/util_tests/huge_alloc_test.cc.o"
	g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/huge_alloc_test.dir/tests/util_tests/huge_alloc_test.cc.o -c /home/s1513031/eRPC/tests/util_tests/huge_alloc_test.cc

CMakeFiles/huge_alloc_test.dir/tests/util_tests/huge_alloc_test.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/huge_alloc_test.dir/tests/util_tests/huge_alloc_test.cc.i"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/s1513031/eRPC/tests/util_tests/huge_alloc_test.cc > CMakeFiles/huge_alloc_test.dir/tests/util_tests/huge_alloc_test.cc.i

CMakeFiles/huge_alloc_test.dir/tests/util_tests/huge_alloc_test.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/huge_alloc_test.dir/tests/util_tests/huge_alloc_test.cc.s"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/s1513031/eRPC/tests/util_tests/huge_alloc_test.cc -o CMakeFiles/huge_alloc_test.dir/tests/util_tests/huge_alloc_test.cc.s

CMakeFiles/huge_alloc_test.dir/tests/util_tests/huge_alloc_test.cc.o.requires:

.PHONY : CMakeFiles/huge_alloc_test.dir/tests/util_tests/huge_alloc_test.cc.o.requires

CMakeFiles/huge_alloc_test.dir/tests/util_tests/huge_alloc_test.cc.o.provides: CMakeFiles/huge_alloc_test.dir/tests/util_tests/huge_alloc_test.cc.o.requires
	$(MAKE) -f CMakeFiles/huge_alloc_test.dir/build.make CMakeFiles/huge_alloc_test.dir/tests/util_tests/huge_alloc_test.cc.o.provides.build
.PHONY : CMakeFiles/huge_alloc_test.dir/tests/util_tests/huge_alloc_test.cc.o.provides

CMakeFiles/huge_alloc_test.dir/tests/util_tests/huge_alloc_test.cc.o.provides.build: CMakeFiles/huge_alloc_test.dir/tests/util_tests/huge_alloc_test.cc.o


# Object files for target huge_alloc_test
huge_alloc_test_OBJECTS = \
"CMakeFiles/huge_alloc_test.dir/tests/util_tests/huge_alloc_test.cc.o"

# External object files for target huge_alloc_test
huge_alloc_test_EXTERNAL_OBJECTS =

build/huge_alloc_test: CMakeFiles/huge_alloc_test.dir/tests/util_tests/huge_alloc_test.cc.o
build/huge_alloc_test: CMakeFiles/huge_alloc_test.dir/build.make
build/huge_alloc_test: build/liberpc.a
build/huge_alloc_test: /usr/lib/libgtest.a
build/huge_alloc_test: CMakeFiles/huge_alloc_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/s1513031/eRPC/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable build/huge_alloc_test"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/huge_alloc_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/huge_alloc_test.dir/build: build/huge_alloc_test

.PHONY : CMakeFiles/huge_alloc_test.dir/build

CMakeFiles/huge_alloc_test.dir/requires: CMakeFiles/huge_alloc_test.dir/tests/util_tests/huge_alloc_test.cc.o.requires

.PHONY : CMakeFiles/huge_alloc_test.dir/requires

CMakeFiles/huge_alloc_test.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/huge_alloc_test.dir/cmake_clean.cmake
.PHONY : CMakeFiles/huge_alloc_test.dir/clean

CMakeFiles/huge_alloc_test.dir/depend:
	cd /home/s1513031/eRPC && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/s1513031/eRPC /home/s1513031/eRPC /home/s1513031/eRPC /home/s1513031/eRPC /home/s1513031/eRPC/CMakeFiles/huge_alloc_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/huge_alloc_test.dir/depend
