# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/catdev/UraniumLang

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/catdev/UraniumLang/build

# Utility rule file for clean_all.

# Include any custom commands dependencies for this target.
include CMakeFiles/clean_all.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/clean_all.dir/progress.make

CMakeFiles/clean_all:
	/usr/bin/cmake -E remove_directory /home/catdev/UraniumLang/build/bin
	/usr/bin/cmake -E remove /home/catdev/UraniumLang/build/library/libulang.a
	/usr/bin/cmake -E remove /home/catdev/UraniumLang/build/compiler/ulang_compiler
	/usr/bin/cmake -E remove /home/catdev/UraniumLang/build/runtime/libulangrt.a

clean_all: CMakeFiles/clean_all
clean_all: CMakeFiles/clean_all.dir/build.make
.PHONY : clean_all

# Rule to build all files generated by this target.
CMakeFiles/clean_all.dir/build: clean_all
.PHONY : CMakeFiles/clean_all.dir/build

CMakeFiles/clean_all.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/clean_all.dir/cmake_clean.cmake
.PHONY : CMakeFiles/clean_all.dir/clean

CMakeFiles/clean_all.dir/depend:
	cd /home/catdev/UraniumLang/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/catdev/UraniumLang /home/catdev/UraniumLang /home/catdev/UraniumLang/build /home/catdev/UraniumLang/build /home/catdev/UraniumLang/build/CMakeFiles/clean_all.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/clean_all.dir/depend

