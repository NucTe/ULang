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

# Include any dependencies generated for this target.
include CMakeFiles/ulang_runtime.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/ulang_runtime.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/ulang_runtime.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/ulang_runtime.dir/flags.make

CMakeFiles/ulang_runtime.dir/runtime/main.cpp.o: CMakeFiles/ulang_runtime.dir/flags.make
CMakeFiles/ulang_runtime.dir/runtime/main.cpp.o: ../runtime/main.cpp
CMakeFiles/ulang_runtime.dir/runtime/main.cpp.o: CMakeFiles/ulang_runtime.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/catdev/UraniumLang/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/ulang_runtime.dir/runtime/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/ulang_runtime.dir/runtime/main.cpp.o -MF CMakeFiles/ulang_runtime.dir/runtime/main.cpp.o.d -o CMakeFiles/ulang_runtime.dir/runtime/main.cpp.o -c /home/catdev/UraniumLang/runtime/main.cpp

CMakeFiles/ulang_runtime.dir/runtime/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ulang_runtime.dir/runtime/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/catdev/UraniumLang/runtime/main.cpp > CMakeFiles/ulang_runtime.dir/runtime/main.cpp.i

CMakeFiles/ulang_runtime.dir/runtime/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ulang_runtime.dir/runtime/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/catdev/UraniumLang/runtime/main.cpp -o CMakeFiles/ulang_runtime.dir/runtime/main.cpp.s

# Object files for target ulang_runtime
ulang_runtime_OBJECTS = \
"CMakeFiles/ulang_runtime.dir/runtime/main.cpp.o"

# External object files for target ulang_runtime
ulang_runtime_EXTERNAL_OBJECTS =

bin/libulangrun.a: CMakeFiles/ulang_runtime.dir/runtime/main.cpp.o
bin/libulangrun.a: CMakeFiles/ulang_runtime.dir/build.make
bin/libulangrun.a: CMakeFiles/ulang_runtime.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/catdev/UraniumLang/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library bin/libulangrun.a"
	$(CMAKE_COMMAND) -P CMakeFiles/ulang_runtime.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ulang_runtime.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/ulang_runtime.dir/build: bin/libulangrun.a
.PHONY : CMakeFiles/ulang_runtime.dir/build

CMakeFiles/ulang_runtime.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/ulang_runtime.dir/cmake_clean.cmake
.PHONY : CMakeFiles/ulang_runtime.dir/clean

CMakeFiles/ulang_runtime.dir/depend:
	cd /home/catdev/UraniumLang/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/catdev/UraniumLang /home/catdev/UraniumLang /home/catdev/UraniumLang/build /home/catdev/UraniumLang/build /home/catdev/UraniumLang/build/CMakeFiles/ulang_runtime.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/ulang_runtime.dir/depend

