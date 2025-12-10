# Install script for directory: C:/Users/vince/OneDrive/Desktop/CS3451/cs3451/assignments

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/cs3451-computer-graphics")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Users/vince/OneDrive/Desktop/CS3451/cs3451/build/assignments/a1/cmake_install.cmake")
  include("C:/Users/vince/OneDrive/Desktop/CS3451/cs3451/build/assignments/a2/cmake_install.cmake")
  include("C:/Users/vince/OneDrive/Desktop/CS3451/cs3451/build/assignments/a3/cmake_install.cmake")
  include("C:/Users/vince/OneDrive/Desktop/CS3451/cs3451/build/assignments/a4/cmake_install.cmake")
  include("C:/Users/vince/OneDrive/Desktop/CS3451/cs3451/build/assignments/a5/cmake_install.cmake")
  include("C:/Users/vince/OneDrive/Desktop/CS3451/cs3451/build/assignments/a6/cmake_install.cmake")
  include("C:/Users/vince/OneDrive/Desktop/CS3451/cs3451/build/assignments/a7/cmake_install.cmake")
  include("C:/Users/vince/OneDrive/Desktop/CS3451/cs3451/build/assignments/a8/cmake_install.cmake")
  include("C:/Users/vince/OneDrive/Desktop/CS3451/cs3451/build/assignments/a9/cmake_install.cmake")

endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "C:/Users/vince/OneDrive/Desktop/CS3451/cs3451/build/assignments/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
