# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_SOURCE_DIR = /home/kao/cpp/CPP-performance-optimization

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/kao/cpp/CPP-performance-optimization/build

# Include any dependencies generated for this target.
include CMakeFiles/Asynchronous_programming.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Asynchronous_programming.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Asynchronous_programming.dir/flags.make

CMakeFiles/Asynchronous_programming.dir/src/Asynchronous_programming.cpp.o: CMakeFiles/Asynchronous_programming.dir/flags.make
CMakeFiles/Asynchronous_programming.dir/src/Asynchronous_programming.cpp.o: ../src/Asynchronous_programming.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kao/cpp/CPP-performance-optimization/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Asynchronous_programming.dir/src/Asynchronous_programming.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Asynchronous_programming.dir/src/Asynchronous_programming.cpp.o -c /home/kao/cpp/CPP-performance-optimization/src/Asynchronous_programming.cpp

CMakeFiles/Asynchronous_programming.dir/src/Asynchronous_programming.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Asynchronous_programming.dir/src/Asynchronous_programming.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/kao/cpp/CPP-performance-optimization/src/Asynchronous_programming.cpp > CMakeFiles/Asynchronous_programming.dir/src/Asynchronous_programming.cpp.i

CMakeFiles/Asynchronous_programming.dir/src/Asynchronous_programming.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Asynchronous_programming.dir/src/Asynchronous_programming.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/kao/cpp/CPP-performance-optimization/src/Asynchronous_programming.cpp -o CMakeFiles/Asynchronous_programming.dir/src/Asynchronous_programming.cpp.s

# Object files for target Asynchronous_programming
Asynchronous_programming_OBJECTS = \
"CMakeFiles/Asynchronous_programming.dir/src/Asynchronous_programming.cpp.o"

# External object files for target Asynchronous_programming
Asynchronous_programming_EXTERNAL_OBJECTS =

Asynchronous_programming: CMakeFiles/Asynchronous_programming.dir/src/Asynchronous_programming.cpp.o
Asynchronous_programming: CMakeFiles/Asynchronous_programming.dir/build.make
Asynchronous_programming: CMakeFiles/Asynchronous_programming.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/kao/cpp/CPP-performance-optimization/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable Asynchronous_programming"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Asynchronous_programming.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Asynchronous_programming.dir/build: Asynchronous_programming

.PHONY : CMakeFiles/Asynchronous_programming.dir/build

CMakeFiles/Asynchronous_programming.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Asynchronous_programming.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Asynchronous_programming.dir/clean

CMakeFiles/Asynchronous_programming.dir/depend:
	cd /home/kao/cpp/CPP-performance-optimization/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/kao/cpp/CPP-performance-optimization /home/kao/cpp/CPP-performance-optimization /home/kao/cpp/CPP-performance-optimization/build /home/kao/cpp/CPP-performance-optimization/build /home/kao/cpp/CPP-performance-optimization/build/CMakeFiles/Asynchronous_programming.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Asynchronous_programming.dir/depend

