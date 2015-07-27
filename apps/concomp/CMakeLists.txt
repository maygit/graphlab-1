#project(GraphLab)
#CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
#list( APPEND CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS} -g -ftest-coverage -fprofile-arcs")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
#cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
#ADD_DEFINITIONS(
#    -std=c++11 # Or -std=c++0x
    # Other flags
#)


# Initialize CXXFLAGS.
#set(CMAKE_CXX_FLAGS                "-Wall -std=c++11")
#set(CMAKE_CXX_FLAGS_DEBUG          "-O0 -g")
#set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
#set(CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
#set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

# Compiler-specific C++11 activation.
#if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
#    execute_process(
#        COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
#    if (NOT (GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7))
#        message(FATAL_ERROR "${PROJECT_NAME} requires g++ 4.7 or greater.")
#    endif ()
#elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
#else ()
#    message(FATAL_ERROR "Your C++ compiler does not support C++11.")
#endif ()

project(GraphLab)
 #set_property(TARGET tgt PROPERTY CXX_STANDARD 11)
#add_executable(foobar main.cc)
#add_executable(rlx_concomp rlx_concomp.cpp)
#target_compile_features(rlx_concomp PRIVATE cxx_range_for)

add_executable(concomp concomp.cpp)
add_executable(sync_concomp sync_concomp.cpp)
add_executable(lock_concomp lock_concomp.cpp)
#add_executable(seq_concomp seq_concomp.cpp)
#add_executable(rlx_concomp rlx_concomp.cpp)
#add_executable(tsv_to_graphlab_bin tsv_to_graphlab_bin.cpp)
#add_executable(pagerankapp pagerankapp.cpp)



#add_mex_library(pagerankmex pagerankapp.cpp  pagerankmex.cpp)
