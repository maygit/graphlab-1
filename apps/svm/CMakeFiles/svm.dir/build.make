# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.6

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canoncical targets will work.
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

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/mzj/mzj_graphlab/graphlab-1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/mzj/mzj_graphlab/graphlab-1

# Include any dependencies generated for this target.
include apps/svm/CMakeFiles/svm.dir/depend.make

# Include the progress variables for this target.
include apps/svm/CMakeFiles/svm.dir/progress.make

# Include the compile flags for this target's objects.
include apps/svm/CMakeFiles/svm.dir/flags.make

apps/svm/CMakeFiles/svm.dir/svm.cpp.o: apps/svm/CMakeFiles/svm.dir/flags.make
apps/svm/CMakeFiles/svm.dir/svm.cpp.o: apps/svm/svm.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/mzj/mzj_graphlab/graphlab-1/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object apps/svm/CMakeFiles/svm.dir/svm.cpp.o"
	cd /home/mzj/mzj_graphlab/graphlab-1/apps/svm && /usr/local/bin/g++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/svm.dir/svm.cpp.o -c /home/mzj/mzj_graphlab/graphlab-1/apps/svm/svm.cpp

apps/svm/CMakeFiles/svm.dir/svm.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/svm.dir/svm.cpp.i"
	cd /home/mzj/mzj_graphlab/graphlab-1/apps/svm && /usr/local/bin/g++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/mzj/mzj_graphlab/graphlab-1/apps/svm/svm.cpp > CMakeFiles/svm.dir/svm.cpp.i

apps/svm/CMakeFiles/svm.dir/svm.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/svm.dir/svm.cpp.s"
	cd /home/mzj/mzj_graphlab/graphlab-1/apps/svm && /usr/local/bin/g++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/mzj/mzj_graphlab/graphlab-1/apps/svm/svm.cpp -o CMakeFiles/svm.dir/svm.cpp.s

apps/svm/CMakeFiles/svm.dir/svm.cpp.o.requires:
.PHONY : apps/svm/CMakeFiles/svm.dir/svm.cpp.o.requires

apps/svm/CMakeFiles/svm.dir/svm.cpp.o.provides: apps/svm/CMakeFiles/svm.dir/svm.cpp.o.requires
	$(MAKE) -f apps/svm/CMakeFiles/svm.dir/build.make apps/svm/CMakeFiles/svm.dir/svm.cpp.o.provides.build
.PHONY : apps/svm/CMakeFiles/svm.dir/svm.cpp.o.provides

apps/svm/CMakeFiles/svm.dir/svm.cpp.o.provides.build: apps/svm/CMakeFiles/svm.dir/svm.cpp.o
.PHONY : apps/svm/CMakeFiles/svm.dir/svm.cpp.o.provides.build

# Object files for target svm
svm_OBJECTS = \
"CMakeFiles/svm.dir/svm.cpp.o"

# External object files for target svm
svm_EXTERNAL_OBJECTS =

apps/svm/svm: apps/svm/CMakeFiles/svm.dir/svm.cpp.o
apps/svm/svm: /usr/lib64/libboost_regex-mt.so
apps/svm/svm: /usr/lib64/libboost_date_time-mt.so
apps/svm/svm: /usr/lib64/libboost_system-mt.so
apps/svm/svm: /usr/lib64/libboost_filesystem-mt.so
apps/svm/svm: /usr/lib64/libboost_thread-mt.so
apps/svm/svm: /usr/lib64/libboost_graph-mt.so
apps/svm/svm: /usr/lib64/libboost_program_options-mt.so
apps/svm/svm: /usr/lib64/libboost_program_options-mt.so
apps/svm/svm: src/graphlab/libgraphlab.a
apps/svm/svm: src/graphlab/extern/metis/libmetis/libgraphlab_metis.a
apps/svm/svm: src/graphlab/extern/metis/GKlib/libgraphlab_GKlib.a
apps/svm/svm: /usr/lib64/libboost_regex-mt.so
apps/svm/svm: /usr/lib64/libboost_date_time-mt.so
apps/svm/svm: /usr/lib64/libboost_system-mt.so
apps/svm/svm: /usr/lib64/libboost_filesystem-mt.so
apps/svm/svm: /usr/lib64/libboost_thread-mt.so
apps/svm/svm: /usr/lib64/libboost_graph-mt.so
apps/svm/svm: /usr/lib64/libboost_program_options-mt.so
apps/svm/svm: apps/svm/CMakeFiles/svm.dir/build.make
apps/svm/svm: apps/svm/CMakeFiles/svm.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable svm"
	cd /home/mzj/mzj_graphlab/graphlab-1/apps/svm && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/svm.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
apps/svm/CMakeFiles/svm.dir/build: apps/svm/svm
.PHONY : apps/svm/CMakeFiles/svm.dir/build

apps/svm/CMakeFiles/svm.dir/requires: apps/svm/CMakeFiles/svm.dir/svm.cpp.o.requires
.PHONY : apps/svm/CMakeFiles/svm.dir/requires

apps/svm/CMakeFiles/svm.dir/clean:
	cd /home/mzj/mzj_graphlab/graphlab-1/apps/svm && $(CMAKE_COMMAND) -P CMakeFiles/svm.dir/cmake_clean.cmake
.PHONY : apps/svm/CMakeFiles/svm.dir/clean

apps/svm/CMakeFiles/svm.dir/depend:
	cd /home/mzj/mzj_graphlab/graphlab-1 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/mzj/mzj_graphlab/graphlab-1 /home/mzj/mzj_graphlab/graphlab-1/apps/svm /home/mzj/mzj_graphlab/graphlab-1 /home/mzj/mzj_graphlab/graphlab-1/apps/svm /home/mzj/mzj_graphlab/graphlab-1/apps/svm/CMakeFiles/svm.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : apps/svm/CMakeFiles/svm.dir/depend

