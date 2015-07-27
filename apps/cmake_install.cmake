# Install script for directory: /home/mzj/mzj_graphlab/graphlab-1/apps

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

IF(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  INCLUDE("/home/mzj/mzj_graphlab/graphlab-1/apps/CMakeFiles/cmake_install.cmake")
  INCLUDE("/home/mzj/mzj_graphlab/graphlab-1/apps/shooting/cmake_install.cmake")
  INCLUDE("/home/mzj/mzj_graphlab/graphlab-1/apps/gabp/cmake_install.cmake")
  INCLUDE("/home/mzj/mzj_graphlab/graphlab-1/apps/demo/cmake_install.cmake")
  INCLUDE("/home/mzj/mzj_graphlab/graphlab-1/apps/image_denoise/cmake_install.cmake")
  INCLUDE("/home/mzj/mzj_graphlab/graphlab-1/apps/svm/cmake_install.cmake")
  INCLUDE("/home/mzj/mzj_graphlab/graphlab-1/apps/pagerank/cmake_install.cmake")
  INCLUDE("/home/mzj/mzj_graphlab/graphlab-1/apps/concomp/cmake_install.cmake")
  INCLUDE("/home/mzj/mzj_graphlab/graphlab-1/apps/factor_graph_inference/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)

