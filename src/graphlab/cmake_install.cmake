# Install script for directory: /home/mzj/mzj_graphlab/graphlab-1/src/graphlab

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "0")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/mzj/mzj_graphlab/graphlab-1/src/graphlab/libgraphlab.a")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  INCLUDE("/home/mzj/mzj_graphlab/graphlab-1/src/graphlab/engine/cmake_install.cmake")
  INCLUDE("/home/mzj/mzj_graphlab/graphlab-1/src/graphlab/graph/cmake_install.cmake")
  INCLUDE("/home/mzj/mzj_graphlab/graphlab-1/src/graphlab/logger/cmake_install.cmake")
  INCLUDE("/home/mzj/mzj_graphlab/graphlab-1/src/graphlab/monitoring/cmake_install.cmake")
  INCLUDE("/home/mzj/mzj_graphlab/graphlab-1/src/graphlab/parallel/cmake_install.cmake")
  INCLUDE("/home/mzj/mzj_graphlab/graphlab-1/src/graphlab/schedulers/cmake_install.cmake")
  INCLUDE("/home/mzj/mzj_graphlab/graphlab-1/src/graphlab/serialization/cmake_install.cmake")
  INCLUDE("/home/mzj/mzj_graphlab/graphlab-1/src/graphlab/scope/cmake_install.cmake")
  INCLUDE("/home/mzj/mzj_graphlab/graphlab-1/src/graphlab/shared_data/cmake_install.cmake")
  INCLUDE("/home/mzj/mzj_graphlab/graphlab-1/src/graphlab/tasks/cmake_install.cmake")
  INCLUDE("/home/mzj/mzj_graphlab/graphlab-1/src/graphlab/util/cmake_install.cmake")
  INCLUDE("/home/mzj/mzj_graphlab/graphlab-1/src/graphlab/extern/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)

