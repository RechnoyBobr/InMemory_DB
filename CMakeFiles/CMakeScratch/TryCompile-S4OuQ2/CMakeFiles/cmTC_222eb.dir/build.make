# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.30

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

# Produce verbose output by default.
VERBOSE = 1

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
CMAKE_SOURCE_DIR = /home/ilya/InMemory_DB/CMakeFiles/CMakeScratch/TryCompile-S4OuQ2

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ilya/InMemory_DB/CMakeFiles/CMakeScratch/TryCompile-S4OuQ2

# Include any dependencies generated for this target.
include CMakeFiles/cmTC_222eb.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/cmTC_222eb.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/cmTC_222eb.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/cmTC_222eb.dir/flags.make

CMakeFiles/cmTC_222eb.dir/CMakeCCompilerABI.c.o: CMakeFiles/cmTC_222eb.dir/flags.make
CMakeFiles/cmTC_222eb.dir/CMakeCCompilerABI.c.o: /usr/share/cmake/Modules/CMakeCCompilerABI.c
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --progress-dir=/home/ilya/InMemory_DB/CMakeFiles/CMakeScratch/TryCompile-S4OuQ2/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/cmTC_222eb.dir/CMakeCCompilerABI.c.o"
	/usr/sbin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/cmTC_222eb.dir/CMakeCCompilerABI.c.o -c /usr/share/cmake/Modules/CMakeCCompilerABI.c

CMakeFiles/cmTC_222eb.dir/CMakeCCompilerABI.c.i: cmake_force
	@echo "Preprocessing C source to CMakeFiles/cmTC_222eb.dir/CMakeCCompilerABI.c.i"
	/usr/sbin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /usr/share/cmake/Modules/CMakeCCompilerABI.c > CMakeFiles/cmTC_222eb.dir/CMakeCCompilerABI.c.i

CMakeFiles/cmTC_222eb.dir/CMakeCCompilerABI.c.s: cmake_force
	@echo "Compiling C source to assembly CMakeFiles/cmTC_222eb.dir/CMakeCCompilerABI.c.s"
	/usr/sbin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /usr/share/cmake/Modules/CMakeCCompilerABI.c -o CMakeFiles/cmTC_222eb.dir/CMakeCCompilerABI.c.s

# Object files for target cmTC_222eb
cmTC_222eb_OBJECTS = \
"CMakeFiles/cmTC_222eb.dir/CMakeCCompilerABI.c.o"

# External object files for target cmTC_222eb
cmTC_222eb_EXTERNAL_OBJECTS =

cmTC_222eb: CMakeFiles/cmTC_222eb.dir/CMakeCCompilerABI.c.o
cmTC_222eb: CMakeFiles/cmTC_222eb.dir/build.make
cmTC_222eb: CMakeFiles/cmTC_222eb.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --progress-dir=/home/ilya/InMemory_DB/CMakeFiles/CMakeScratch/TryCompile-S4OuQ2/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable cmTC_222eb"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/cmTC_222eb.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/cmTC_222eb.dir/build: cmTC_222eb
.PHONY : CMakeFiles/cmTC_222eb.dir/build

CMakeFiles/cmTC_222eb.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/cmTC_222eb.dir/cmake_clean.cmake
.PHONY : CMakeFiles/cmTC_222eb.dir/clean

CMakeFiles/cmTC_222eb.dir/depend:
	cd /home/ilya/InMemory_DB/CMakeFiles/CMakeScratch/TryCompile-S4OuQ2 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ilya/InMemory_DB/CMakeFiles/CMakeScratch/TryCompile-S4OuQ2 /home/ilya/InMemory_DB/CMakeFiles/CMakeScratch/TryCompile-S4OuQ2 /home/ilya/InMemory_DB/CMakeFiles/CMakeScratch/TryCompile-S4OuQ2 /home/ilya/InMemory_DB/CMakeFiles/CMakeScratch/TryCompile-S4OuQ2 /home/ilya/InMemory_DB/CMakeFiles/CMakeScratch/TryCompile-S4OuQ2/CMakeFiles/cmTC_222eb.dir/DependInfo.cmake
.PHONY : CMakeFiles/cmTC_222eb.dir/depend
